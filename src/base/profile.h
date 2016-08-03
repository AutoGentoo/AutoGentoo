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


#include <stdio.h>
#include <cglobal/mstring.h>
#include <cglobal/command_tools.h>

#ifndef __AUTOGENTOO_PROFILE_H__
#define __AUTOGENTOO_PROFILE_H__

/**
 * Current supported profiles:
 * default/linux/amd64/13.0/
 * default/linux/amd64/13.0/systemd
 * default/linux/amd64/13.0/desktop/gnome/systemd
**/

typedef struct
{
  bool not_valid;
  
  mstring     full;
  mstring     mainType; /** Default, Hardened, etc. **/
  mstring     archType; /** AMD64, X86, musl, ulibc **/
  
  /** Binhost available:
   * gnome & gnome/systemd (GTK)
   * kde & kde/systemd (QT4)
   * plasma & plasma/systemd (QT5)
   */
  bool        isdesktop;
  mstring     desktop; /** xfce, gnome, kde, plasma **/
  mstring     init; /** openrc, systemd **/
  
  /** Only for non-desktop systems **/
  bool        has_variant;
  mstring     varaint; /** selinux, developer, no-multilib, systemd, x32 **/
  mstring     second_varaint; /** selinux **/
  
} Profile;

typedef struct
{
  Profile        *profiles;
  int             profilec;
  int             active;
  mstring         active_name;
} ProfileList;

Profile             profile_new            (void);

Profile             profile_new_from_str   (mstring);

void                profilelist_get_list   (ProfileList*);

void                profilelist_free       (ProfileList*);

void                profile_free           (Profile*);

#endif
