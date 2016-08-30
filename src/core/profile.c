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
#include <core/profile.h>

Profile*
profile_new_from_str (char* in)
{
  Profile* out = malloc (sizeof(Profile));
  char**  divided = mstring_split (in, '/');
  snprintf (out->full, 64, "%s", in);
  snprintf (out->main, 16, "%s", divided[0]);
  snprintf (out->arch, 16, "%s", divided[2]);
  
  if (!divided[3])
  {
    //fprintf (stderr, "Profile %s not supported, skipping...\n", in);
    out->valid = FALSE;
    free (divided);
    return out;
  }
  
  if (strcmp (divided[3], "13.0") != 0)
  {
    //fprintf (stderr, "Profile %s not supported, skipping...\n", in);
    out->valid = FALSE;
    free (divided);
    return out;
  }
  out->valid = TRUE;
  
  if (divided[4] == NULL)
  {
    out->has_variant = FALSE;
    out->isdesktop   = FALSE;
    if (divided[5])
    {
      snprintf (out->varaint, 16,"%s", divided[5]);
    }
    if (divided[6])
    {
      snprintf (out->second_varaint, 16,"%s", divided[6]);
    }
  }
  else
  {
    if (strcmp (divided[4], "desktop") != 0)
    {
    out->has_variant = FALSE;
      out->isdesktop   = FALSE;
      if (divided[5])
      {
        snprintf (out->varaint, 16,"%s", divided[5]);
      }
      if (divided[6])
      {
        snprintf (out->second_varaint, 16,"%s", divided[6]);
      }
    }
    else
    {
      out->isdesktop = TRUE;
      if (!divided[5])
      {
        strcpy(out->desktop, "xfce");
      }
      else
      {
        strcpy(out->desktop, divided[5]);
      }
      
      if (!divided[6])
      {
        strcpy(out->init, "openrc");
      }
      else
      {
        strcpy(out->init, divided[6]);
      }
    }
  }
  
  free (divided);
  return out;
}

ProfileList* profilelist_new ()
{
  ProfileList* out = malloc (sizeof (ProfileList));
  out->profiles = malloc (sizeof(Profile*) * 128);
  
  return out;
};

void
profilelist_get_list (ProfileList * out)
{
  char** profiles = get_output_lines ("eselect --brief profile list");
  char** used_profiles = palloc (sizeof(char*) * 128);
  char* active_buff = get_output ("eselect --brief --colour=no profile show | cut -c 3-100");
  
  int curr;
  int p_curr;
  for (curr = 0, p_curr = 0; profiles[curr]; curr++)
  {
    Profile* p_buff = profile_new_from_str (profiles[curr]);
    
    if (!p_buff->valid)
    {
      continue;
    }
    
    if (strcmp (p_buff->full, active_buff) == 0)
    {
      out->active = p_buff;
    }
    
    out->profiles [p_curr] = p_buff;
    used_profiles[p_curr] = profiles[curr];
    p_curr++;
    out->count++;
  }
  
  array_free ((void**)profiles);
}

void profilelist_free (ProfileList* ptr)
{
  array_free ((void**)ptr->profiles);
  //free (ptr);
}
