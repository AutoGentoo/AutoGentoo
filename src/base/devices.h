/*
 * devices.h
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos-Ubuntu>
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

#ifndef __AUTOGENTOO_DEVICES_H__
#define __AUTOGENTOO_DEVICES_H__

#include <stdio.h>
#include <stdlib.h>
#include <cglobal/command_tools.h>

typedef struct __DISK Disk;
typedef struct __PART Partition;
typedef struct __FREE Free;
typedef struct __STOR Storage;

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

Partition part_new_from_str      (char**, Disk*);

Free      free_new_from_str      (char**, Disk*);

Disk      disk_new_from_path     (char*);

void      probe_devices          (Storage*);

Disk      get_disk               (Storage*, char*);

Partition get_partition          (Storage*, char*);

Storage*  storage_new            ();

void      storage_free           (Storage*);

#endif
