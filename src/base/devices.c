/*
 * devices.c
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <base/devices.h>

/*
parted --machine --script --align=opt /dev/sda unit b p free
OUTPUT:
UNIT;
"path":"size":"transport-type":"logical-sector-size":"physical-sector-size":"partition-table-type":"model-name";

(for BYT)
"number":"begin":"end":"size":"filesystem-type":"partition-name":"flags-set";

(for CHS/CYL)
"number":"begin":"end":"filesystem-type":"partition-name":"flags-set";

(for free)
1:"begin":"end":"size":free;
*/


struct __PART
{
  char      path[15];
  int       num;
  Disk     *parent;
  int       type; /* Primary:0, Extended:1, Logical:2 */
  char      fstype[25];
  char      name[128];
  char    **flags;
  
  char      mount_point[1024];
  
  /* In BYTES */
  int       start;
  int       end;
  int       size;
  
};

struct __FREE
{
  int       start;
  int       end;
  int       size;
  
  Disk     *parent;
  
};

struct __DISK
{
  char    path[10];
  int     size;
  char    transport[24]; /* scsi */
  int     logical_sec;
  int     physical_sec;
  char    table[10]; /* msdos, gpt */
  char    model[1024];
  
  char    UNIT[3]; /* CHS, CYL, BYT */
  
  int        part_types[128]; /* Free:0, Partition: 1 */
  Partition  partitions[128];
  Free       freespaces[128];
  
};

struct __STOR
{
  Disk      *disks;
};

Partition part_new_from_str (char** splt, Disk* parent)
{
  Partition out;
  out.parent = parent;
  snprintf (out.path, 16, "%s%s", parent->path, splt[0]);
  sscanf (splt[0], "%d", &out.num);
  sscanf (splt[1], "%d", &out.start);
  sscanf (splt[2], "%d", &out.end);
  
  if (strcmp (&parent->UNIT[0], "BYT") == 0)
  {
    sscanf (splt[3], "%d", &out.size);
    snprintf (out.fstype, 26, "%s", splt[4]);
    snprintf (out.name, 129, "%s", splt[5]);
  }
  else
  {
    snprintf (out.fstype, 26, "%s", splt[3]);
    snprintf (out.name, 129, "%s", splt[4]);
    out.size = out.end - out.start;
  }
  return out;
}

Free free_new_from_str (char** splt, Disk* parent)
{
  Free out;
  out.parent = parent;
  
  sscanf (splt[1], "%d", &out.start);
  sscanf (splt[2], "%d", &out.end);
  sscanf (splt[3], "%d", &out.size);
  
  return out;
}

Disk disk_new_from_path (char* path)
{
  Disk out;
  char *cmd = malloc (sizeof(char) * mstring_get_length ("parted --machine --script --align=opt %s unit /dev/sdaaaa print free"));
  sprintf (cmd, "parted --machine --script --align=opt %s unit b print free", path);
  
  char** cmd_buff = get_output_lines (cmd);
  char** disk_buff = mstring_split (cmd_buff[1], ':');
  
  snprintf (out.UNIT, 4, "%s", cmd_buff[0]);
  snprintf (out.path, 11, "%s", disk_buff[0]);
  sscanf (disk_buff[1], "%d", &out.size);
  snprintf (out.transport, 25, "%s", disk_buff[2]);
  sscanf (disk_buff[3], "%d", &out.logical_sec);
  sscanf (disk_buff[4], "%d", &out.physical_sec);
  snprintf (out.table, 11, "%s", disk_buff[5]);
  snprintf (out.model, 1025, "%s", disk_buff[6]);
  
  int curr = 2;
  int curr_p = 0;
  
  int curr_p_num = 0;
  int curr_f_num = 0;
  
  for (; curr != get_output_length (cmd); curr++, curr_p++)
  {
    print ("==========%s=========", cmd_buff[curr]);
    char** part_splt = mstring_split (cmd_buff[curr], ':');
    aprint (part_splt);
    print ("passed part_splt");
    fflush (stdout);
    char * cmpbuff = mstring_get_sub_py (part_splt[4], 0, 4);
    print ("==========%s=========", cmd_buff[curr]);
    if (strcmp (cmpbuff, "free") == 0)
    {
      out.part_types[curr_p] = 0;
      out.freespaces[curr_f_num] = free_new_from_str (part_splt, &out);
      curr_f_num++;
    }
    else
    {
      out.part_types[curr_p] = 1;
      out.partitions[curr_p_num] = part_new_from_str (part_splt, &out);
      curr_p_num++;
    }
    free (cmpbuff);
    mstring_a_free (part_splt);
  }
  print ("-------");
  mstring_a_free (cmd_buff);
  mstring_a_free (disk_buff);
  free(cmd);
  return out;
}


Storage* storage_new ()
{
  Storage *out = malloc (sizeof(Storage));
  char** lns = get_output_lines ("ls /sys/block | grep -v loop");
  int disknum = mstring_a_get_length (lns);
  mstring_a_free (lns);
  out->disks = malloc (sizeof(Disk) * (disknum+1));
  return out;
}

void storage_free (Storage* ptr)
{
  free(ptr->disks);
  free(ptr);
}

void probe_devices (Storage* out)
{
  char** blks = get_output_lines ("lsblk --raw");
  int curr_dev = 1;
  int curr_disk = 0;
  for (curr_dev = 1; blks[curr_dev]; curr_dev++)
  {
    char** splt = mstring_split(blks[curr_dev], ' ');
    if (strcmp (splt[5], "disk") == 0)
    {
      char b[10];
      snprintf (b, 10, "/dev/%s", splt[0]);
      out->disks[curr_disk] = disk_new_from_path (b);
      curr_disk++;
    }
    mstring_a_free (splt);
  }
  
  mstring_a_free (blks);
}
