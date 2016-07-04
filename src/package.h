/*
 * package.h
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
#include <string.h>
#include "mstring.h"

typedef struct
{
  bool        new;
  bool        slot;
  bool        updating;
  bool        downgrading;
  bool        reinstall;
  bool        replacing;
  bool        fetch_man;
  bool        fetch_auto;
  bool        interactive;
  bool        blocked_man;
  bool        blocked_auto;
} PkgProperty;

typedef struct
{
  mstring     revision;
  mstring     version;
  mstring     repo;
  mstring     full;
  mstring     slot;
} PkgVersion;

typedef struct
{
  mstring           type; //ebuild, block
  PkgProperty      *flags;
  mstring           category;
  mstring           name;
  PkgVersion       *version;
  PkgVersion       *old;
  mstring_a         vars;
  mstring_a         vals;
  mstring           file;
  mstring           name_raw;
  mstring           size;
} Package;

Package  *        package_new                (void);

Package  *        package_new_from_string    (mstring);

int               package_do_stage           (Package*, mstring);

PkgProperty *     pkgproperty_new            (void);

PkgVersion  *     pkgversion_new             (void);

PkgVersion  *     pkgversion_new_from_str    (mstring);

void              pkgproperty_reset          (PkgProperty*);

void              pkgversion_parse           (PkgVersion*, mstring);
