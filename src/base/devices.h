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
#include <cglobal/mstring.h>
#include <cglobal/command_tools.h>

typedef struct __DISK Disk;
typedef struct __PART Partition;
typedef struct __FREE Free;
typedef struct __STOR Storage;

Partition part_new               ();

void      part_free              (Partition*);

Partition part_new_from_str      (char**, Disk*);

Free      free_new_from_str      (char**, Disk*);

Disk      disk_new               ();

void      disk_free              (Disk*);

Disk      disk_new_from_path     (char*);

void      probe_devices          (Storage*);

Disk      get_disk               (Storage*, char*);

Partition get_partition          (Storage*, char*);

Storage*  storage_new            ();

void      storage_free           (Storage*);

#endif
