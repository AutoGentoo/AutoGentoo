/*
 * command_tools.c
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


#include "command_tools.h"

char*
get_output (char* cmd_in)
{
  char cmd[mstring_get_length (cmd_in) + 15];
  snprintf (cmd, mstring_get_length (cmd_in) + 15, "%s > cmd.temp", cmd_in);
  system (cmd);
  char* buff = read_file ("cmd.temp");
  system ("rm -rf cmd.temp");
  return buff;
}

int
get_output_length (char* __command)
{
  char cmd [mstring_get_length (__command) + 15];
  snprintf (cmd, mstring_get_length (__command) + 15, "%s > cmd.temp", __command);
  system (cmd);
  int out = getlength ("cmd.temp");
  system("rm -rf cmd.temp");
  
  return out;
}

char**
get_output_lines (char* cmd_in)
{
  char cmd [mstring_get_length (cmd_in) + 15];
  snprintf (cmd, mstring_get_length (cmd_in) + 15, "%s > cmd.temp", cmd_in);
  system (cmd);
  char** lines = readlines ("cmd.temp");
  system("rm -rf cmd.temp");
  return lines;
}
