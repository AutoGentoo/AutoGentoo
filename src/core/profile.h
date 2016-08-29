/*
 * profile.h
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


#ifndef __AUTOGENTOO_PROFILE_H__
#define __AUTOGENTOO_PROFILE_H__

#include <stdio.h>
#include <cglobal/command_tools.h>

/**
 * Current supported profiles:
 * default/linux/amd64/13.0/ (cinnamon/none)
 * default/linux/amd64/13.0/systemd (cinnamon/none)
 * default/linux/amd64/13.0/desktop/gnome/systemd (gnome)
**/

typedef struct
{
  int         valid
  char        full[64];
  char        main[16]; /** Default, Hardened, etc. **/
  char        arch[16]; /** AMD64, X86, musl, ulibc **/
  
  int         isdesktop;
  char        desktop[16]; /** xfce, gnome, kde, plasma, cinnamon **/
  char        init[16]; /** openrc, systemd **/
  
  /** Only for non-desktop systems **/
  int         has_variant;
       varaint; /** selinux, developer, no-multilib, systemd, x32 **/
  mstring     second_varaint; /** selinux **/
  
} Profile;

typedef struct
{
  Profile**       profiles;
  int             count;
  Profile*        active;
} ProfileList;

#endif
