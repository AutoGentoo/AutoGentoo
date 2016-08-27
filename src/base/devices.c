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
  char     *path;
  int       num;
  Disk     *parent;
  int       type; /* Primary:0, Extended:1, Logical:2 */
  char     *fstype;
  char     *name;
  char    **flags;
  
  char     *mount_point;
  
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
  char   *path;
  int     size;
  char   *transport; /* scsi */
  int     logical_sec;
  int     physical_sec;
  char   *table; /* msdos, gpt */
  char   *model;
  
  char   *UNIT; /* CHS, CYL, BYT */
  
  int       *part_types; /* Free:0, Partition: 1 */
  Partition *partitions;
  Free      *freespaces;
  
};

struct __STOR
{
  Disk      *disks;
};

Partition part_new ()
{
  Partition out;
  out.path  = malloc (sizeof(char) * 12);
  out.fstype= malloc (sizeof(char) * 20);
  out.num   = 0;
  out.type  = 0;
  out.size  = 0;
  
  return out;
}

void part_free (Partition* ptr)
{
  free(ptr->path);
  free(ptr->fstype);
}

Partition part_new_from_str (char** splt, Disk* parent)
{
  Partition out = part_new ();
  
  //char** buff = mstring_split (mstring_get_sub_py (partline, 0 -2), ',');
  
  sprintf (out.path, "%s%s", parent->path, splt[0]);
  sscanf (splt[0], "%d", &out.num);
  sscanf (splt[1], "%d", &out.start);
  sscanf (splt[2], "%d", &out.end);
  
  if (strcmp (parent->UNIT, "BYT") == 0)
  {
    sscanf (splt[3], "%d", &out.size);
    strcpy (out.fstype, splt[4]);
    strcpy (out.name, splt[5]);
    out.flags = mstring_split_str (splt[6], ", ");
  }
  else
  {
    strcpy (out.fstype, splt[3]);
    strcpy (out.name, splt[4]);
    out.flags = mstring_split_str (splt[5], ", ");
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

Disk disk_new ()
{
  Disk out;
  out.path          = malloc (sizeof(char) * 10);
  out.transport     = malloc (sizeof(char) * 24);
  out.table         = malloc (sizeof(char) * 10);
  out.model         = malloc (sizeof(char) * 1024);
  out.UNIT          = malloc (sizeof(char) * 3);
  out.part_types    = malloc (sizeof(int)  * 128);
  out.partitions    = malloc (sizeof(Partition) * 128);
  out.freespaces    = malloc (sizeof(Free)      * 128);
  
  return out;
}

void disk_free (Disk* ptr)
{
  free (ptr->path);
  free (ptr->transport);
  free (ptr->table);
  free (ptr->model);
  free (ptr->UNIT);
  free (ptr->part_types);
  free (ptr->partitions);
  free (ptr->freespaces);
  
  int i;
  for (i=0; &ptr->partitions[i]; i++)
  {
    part_free (&ptr->partitions[i]);
  }
  
  free(ptr->partitions);
  free(ptr->freespaces);
}

Disk disk_new_from_path (char* path)
{
  Disk out = disk_new ();
  
  char** cmd_buff = get_output_lines (rprintf ("parted --machine --script --align=opt %s unit b print free", path));
  char** disk_buff = mstring_split (cmd_buff[1], ':');
  
  printf ("start disk info\n");
  out.UNIT = cmd_buff[0];
  out.path = disk_buff[0];
  sscanf (disk_buff[1], "%d", &out.size);
  out.transport = disk_buff[2];
  sscanf (disk_buff[3], "%d", &out.logical_sec);
  sscanf (disk_buff[4], "%d", &out.physical_sec);
  out.table = disk_buff[5];
  out.model = disk_buff[6];
  printf ("passed disk info\n");
  fflush(stdout);
  int curr = 2;
  int curr_p = 0;
  
  int curr_p_num = 0;
  int curr_f_num = 0;
  
  for (; cmd_buff[curr]; curr++, curr_p++)
  {
    char* curr_part = cmd_buff[curr];
    char** part_splt = mstring_split (curr_part, ':');
    
    if (strcmp (part_splt[4], "free") == 0)
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
  }
  mstring_a_free (cmd_buff);
  
  return out;
}


Storage* storage_new ()
{
  Storage *out = malloc (sizeof(Storage));
  char** lns = get_output_lines ("ls /sys/block | grep -v loop");
  int disknum = mstring_a_get_length (lns);
  mstring_a_free (lns);
  out->disks = malloc (sizeof(Disk) * disknum);
  return out;
}

void storage_free (Storage* ptr)
{
  int i;
  for (i=0; &ptr->disks[i]; i++)
  {
    disk_free (&ptr->disks[i]);
  }
  free(ptr->disks);
  free(ptr);
}

void probe_devices (Storage* out)
{
  out = storage_new ();
  
  char** blks = get_output_lines ("lsblk --raw");
  int curr_dev = 1;
  int curr_disk = 0;
  for (; blks[curr_dev]; curr_dev++)
  {
    char** splt = mstring_split(blks[curr_dev], ' ');
    if (strcmp (splt[5], "disk") == 0)
    {
      out->disks[curr_disk] = disk_new_from_path (rprintf ("/dev/%s", splt[0]));
      curr_disk++;
    }
  }
  
  mstring_a_free (blks);
}
