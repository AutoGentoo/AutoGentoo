/*
 * autogentoo.h
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


#include <stdio.h>

typedef struct
{
  /* System */
  char      *arch;
  bool       specify_stage3; /* Not recommended */
  char      *stage3;
  
  /* Pre-chroot */
  Device    *devices;
  Stage3     source-stg3;
  char     **mount_cmds; /* Proc, dev, sys (for chroot) */
  
  /* Packages */
  bool       use_binhost;
  char      *binhost_addr;
  char      *binhost_sync;
  char      *binhost_ssh_key;
  char     **to_install;
  Profile    profile;
  char     **extra_install;
  
  /* Graphics */
  char      *gpu;
  bool       graphicsInKernel;
  bool       has_splash;
  
  /* Kernel */
  char     *Kconfig;
  bool      config_provided;
  Kernel    kernel;
  
  /* Init */
  bool     is_systemd;
  char    *addToGrub;
  
  /* Users */
  char    *user;
  char   **user_groups;
  char    *user_passwd;
  char    *root_passwd;
  char    *hostname;
  
  /* Timezone/Locale */
  char    *timezone;
  char    *locale;
} ag_system;

ag_system             ag_system_new             (void);

ag_system             ag_system_new_from_file   (char*);

int                   ag_system_download_stg    (ag_system);

int                   ag_system_extract_stg     (ag_system);

int                   ag_system_mount_main      (ag_system);

int                   ag_system_mount_chroot    (ag_system);

int                   ag_system_sync            (ag_system, int);

int                   ag_system_extract         (ag_system);

int                   ag_system_install         (ag_system);

int                   ag_system_install_extra   (ag_system);

int                   ag_system_kernel          (ag_system);

int                   ag_system_systemd         (ag_system);

int                   ag_system_grub            (ag_system);

//int                   ag_system_
