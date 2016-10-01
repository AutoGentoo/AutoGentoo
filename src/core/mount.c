/*
 * mount.c
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


#include <core/mount.h>

char* chroot_cmds[] = {
    "mount -t proc /proc proc",
    "mount -R /dev dev",
    "mount -R /sys sys"};

struct __mount* mount (char* dev, char* dest)
{
  struct __mount* out = malloc (sizeof (struct __mount));
  out->dev = dev;
  out->dest = dest;
  out->status = systemf ("mount %s %s", out->dev, out->dest);
  
  return out;
}

int mount_chroot (char* dest)
{
  int i = 0;
  int exit = 0;
  for (; i != 3; i++)
  {
    exit += systemf ("cd %s && %s", dest, chroot_cmds[i]);
  }
  return exit;
}

int umount (struct __mount* __mount__)
{
  int out = systemf ("umount -l %s", __mount__->dest);
  free (__mount__);
  return out;
}

int umount_chroot (char* dest)
{
  int exit;
  exit += systemf ("umount %s/proc", dest);
  exit += systemf ("umount -l %s/dev", dest);
  exit += systemf ("umount -l %s/sys", dest);
  return exit;
}
