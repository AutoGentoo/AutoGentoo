/*
 * disk.h
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

#ifndef __AUTOGENTOO_PARTED_DISK_H__
#define __AUTOGENTOO_PARTED_DISK_H__

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <parted/parted.h>
#include <core/tools/ptr_handler.h>

struct Disk
{
  PedDevice*      p_device;
  PedDisk*        p_disk;
  PedPartition**  partitions;
};

extern int TOTAL_DISKS;

struct Disk** disk_get_all (void);

void disk_free (struct Disk** ptr);

#endif
