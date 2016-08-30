/*
 * disk.c
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
#include <core/disk.h>

int TOTAL_DISKS = 0;

struct Disk** disk_get_all (void)
{
  struct passwd *p = getpwuid(getuid());
  
  if (p == NULL)
  {
    fprintf (stderr, "Failed to get username!\n");
    exit (1);
  }
  if (strcmp (p->pw_name, "root") != 0)
  {
    fprintf (stderr, "Not running as root!\n");
    exit (1);
  }
  
  struct Disk** out = malloc (sizeof(struct Disk*) * 128);
  
  ped_device_probe_all ();
  
  /* Get the first device */
  PedDevice* last = ped_device_get_next (NULL);
  
  int curr = 0;
  out[curr] = malloc (sizeof(struct Disk));
  out[curr]->p_device = last;
  out[curr]->p_disk = ped_disk_new (out[curr]->p_device);
  out[curr]->partitions = malloc (sizeof(PedPartition*) * 128);
  
  PedPartition* __part_ptr = NULL;
  int i = 0;
  while (__part_ptr != NULL || i == 0)
  {
    __part_ptr = ped_disk_next_partition (out[curr]->p_disk, __part_ptr);
    out[curr]->partitions[i] = __part_ptr;
    i++;
  }
  curr++;
  TOTAL_DISKS++;
  while (last != NULL)
  {
    out[curr] = malloc (sizeof(struct Disk));
    out[curr]->p_device = ped_device_get_next (last);
    if (out[curr]->p_device == NULL)
    {
      break;
    }
    TOTAL_DISKS++;
    out[curr]->p_disk = ped_disk_new (out[curr]->p_device);
    out[curr]->partitions = malloc (sizeof(PedPartition*) * 128);
    
    PedPartition* part_ptr = NULL;
    int i = 0;
    while (part_ptr != NULL || i == 0)
    {
      part_ptr = ped_disk_next_partition (out[curr]->p_disk, part_ptr);
      out[curr]->partitions[i] = part_ptr;
      i++;
    }
    
    last = out[curr]->p_device;
    curr++;
  }
  
  return out;
}

void disk_free (struct Disk** ptr)
{
  int i;
  for (i=0; ptr[i]; i++)
  {
    free_raw (ptr[i]->partitions);
    free_raw (ptr[i]);
  }
  free_raw (ptr);
}
