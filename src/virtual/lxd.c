/*
 * lxd.c
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


#include <virtual/lxd.h>

int
lxd_sync (char * lxd_img, char * dest)
{
  char * command_tar  = malloc (sizeof(char) * strlen("cd /usr/portage/packages && rm -rf compressed.tar.gz && tar cfz compressed.tar.gz ") + );
  char * command_pull = malloc (sizeof(char) * strlen ("lxc file pull   /usr/portage/packages/compressed.tar.gz   ") + strlen(lxd_img) + strlen(dest));
  char * command_untar = malloc (sizeof(char) * strlen ("tar xf   /compressed.tar.gz -C  ") + strlen (dest) + strlen (dest));
  
  sprintf (command_pull, "lxc file pull %s/usr/portage/packages/compressed.tar.gz %s/", lxd_img, dest);
  sprintf (command_untar, "tar xf %s/compressed.tar.gz -C %s", dest, dest);
  
  int exit_status = 0;
  printf ("Compressing binaries into a .tar.gz...\n");
  exit_status = lxd_cmd (lxd_img, command_tar);
  
  if (exit_status != 0)
  {
    fprintf (stderr, "Compression of package binaries failed!\n");
    return exit_status;
  }
  
  printf ("Transfer binaries into host...\n");
  exit_status = system (command_pull);
  free (command_pull);
  
  if (exit_status != 0)
  {
    fprintf (stderr, "Binary transfer failed!\n");
    return exit_status;
  }
  
  printf ("Extracting binaries...\n");
  exit_status = system (command_untar);
  free (command_untar);
  
  if (exit_status != 0)
  {
    fprintf (stderr, "Binary extraction failed!\n");
    return exit_status;
  }
  
  return 0;
}

int
lxd_cmd  (char * lxd_img, char * cmd)
{
  char * command = malloc (sizeof(char) * strlen ("lxc exec %s -- bash -c   ") + strlen(lxd_img) + strlen (cmd));
  
  sprintf (command, "lxc exec %s -- bash -c '%s'", lxd_img, cmd);
  
  int exit_status = 0;
  exit_status = system (command);
  free (command);
  
  return exit_status;
}
