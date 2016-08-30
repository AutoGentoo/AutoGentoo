/*
 * parted_test.c
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

int main(int argc, char **argv)
{
  /*printf ("Scanning disks...");
  fflush (stdout);
  ped_device_probe_all ();
  printf ("done\n");
  fflush (stdout);
  
  PedDevice* devlist [128];
  
  * Get the first device
  PedDevice* first_dev = ped_device_get_next (NULL);
  PedDevice* last = first_dev;
  
  int curr = 0;
  devlist[curr] = last;
  curr++;
  
  while (last != NULL)
  {
    devlist[curr] = ped_device_get_next (last);
    
    last = devlist[curr];
    curr++;
  }
  
  int i;
  for (i=0; devlist[i]; i++)
  {
    PedDisk* peddisk_buffer = ped_disk_new (devlist[i]);
    printf ("Found device at %s\nModel name: %s\n", devlist[i]->path, devlist[i]->model);
    printf ("Disk has label: %s\n", peddisk_buffer->type->name);
    fflush (stdout);
  }
  */
  struct Disk** test = disk_get_all ();
  
  int i;
  for (i=0; i != TOTAL_DISKS; i++)
  {
    printf ("Found disk with path: %s\nModel: %s\nLabel: %s\n", test[i]->p_device->path, test[i]->p_device->model, test[i]->p_disk->type->name);
    fflush(stdout);
  }
  printf ("Found %d disks total\n", TOTAL_DISKS);
  disk_free (test);
  
  
  return 0;
  
  
  
}

