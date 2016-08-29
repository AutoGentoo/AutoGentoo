/*
 * profile.c
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
#include <base/profile.h>

Profile
profile_new (void)
{
  Profile       out;
  out.full      = malloc (sizeof(char) * 1024);
  out.mainType  = malloc (sizeof(char) * 16);
  out.archType  = malloc (sizeof(char) * 16);
  out.desktop   = malloc (sizeof(char) * 16);
  out.init      = malloc (sizeof(char) * 16);
  out.varaint   = malloc (sizeof(char) * 16);
  out.second_varaint= malloc (sizeof(char) * 16);
  
  return out;
}

Profile
profile_new_from_str   (mstring in)
{
  Profile out = profile_new ();
  //"default/linux/amd64/13.0"
  mstring_a  divided = mstring_split (in, '/');
  out.full = in;
  out.mainType = divided[0];
  out.archType = divided[2];
  
  if (strcmp (divided[3], "13.0") != 0)
  {
    fprintf (stderr, "Profile %s not supported, skipping...", in);
    out.not_valid = TRUE;
    return out;
  }
  
  out.not_valid = FALSE;
  
  if (strcmp (divided[4], "desktop") != 0)
  {
    out.varaint = "";
    out.has_variant = FALSE;
    out.isdesktop   = FALSE;
    if (divided[5])
    {
      out.varaint = divided[5];
    }
    if (divided[6])
    {
      out.second_varaint = divided[6];
    }
  }
  else
  {
    out.isdesktop = TRUE;
    if (!divided[5])
    {
      out.desktop = "xfce";
    }
    else
    {
      out.desktop = divided[5];
    }
    
    if (!divided[6])
    {
      out.init = "openrc";
    }
    else
    {
      out.init = divided[6];
    }
  }
  
  free (divided);
  return out;
}

void
profilelist_get_list (ProfileList * out)
{
  system("eselect --brief profile list");
  mstring_a profiles = get_output_lines ("eselect --brief profile list");
  //printf ("%s", profiles[0]);
  mstring_a used_profiles = malloc (sizeof(mstring) * 128);
  
  int curr;
  int p_curr;
  for (curr = 0, p_curr = 0; profiles[curr]; curr++)
  {
    Profile p_buff = profile_new_from_str (profiles[curr]);
    if (p_buff.not_valid == TRUE)
    {
      continue;
    }
    out->profiles [p_curr] = p_buff;
    used_profiles[p_curr] = profiles[curr];
    p_curr++;
    out->profilec++;
  }
  
  out->active_name = mstring_get_sub_py (get_output ("eselect --brief --colour=no profile show"), 2, -1);
  out->active = mstring_a_find (used_profiles, out->active_name);
  
  free(profiles);
  free(used_profiles);
}
