/*
 * main.c
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


#include <gui/main.h>

int file_c = 1;
char* file_order[] = {
  "autogentoo.ui"};

void contruct (struct __AutoGentooMain* main_class)
{
  main_class = malloc (sizeof (struct __AutoGentooMain));
  main_class->builders = malloc (sizeof (GtkBuilder*) * file_c);
  
  int i;
  for (i=0; i != file_c; i++)
  {
    main_class->builders[i] = gtk_builder_new ();
    gtk_builder_add_from_file (main_class->builders[i], file_order[i], NULL);
  }
  
  main_class->main = GTK_WINDOW (gtk_builder_get_object (main_class->builders[0], "installer_window"));
  main_class->top_level = GTK_ADJUSTMENT (gtk_builder_get_object (main_class->builders[0], "top_level"));
  main_class->i = 0;
}

void next (struct __AutoGentooMain* main_class)
{
  gtk_container_remove (GTK_CONTAINER (main_class->main), GTK_WIDGET (main_class->top_level));
  main_class->i++;
  if (main_class->i == file_c)
  {
    printf ("Can't next anymore exiting...\n");
    exit (1);
  }
  main_class->top_level = GTK_ADJUSTMENT (gtk_builder_get_object (main_class->builders[main_class->i], "top_level"));
  gtk_container_add (GTK_CONTAINER (main_class->main), GTK_WIDGET (main_class->top_level));
}

int main (int argc, char** argv)
{
  return 0;
}

//void back (struct __AutoGentooMain*);
