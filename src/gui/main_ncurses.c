/*
 * main_ncurses.c
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
#include <ncurses.h>
#include <form.h>

int main()
{
  FIELD *field[3];
  FORM  *my_form;
  int ch;
  
  /* Initialize curses */
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  /* Initialize the fields */
  field[0] = new_field(1, 10, 4, 18, 0, 0);
  field[1] = new_field(1, 10, 6, 18, 0, 0);
  field[2] = NULL;

  /* Set field options */
  set_field_back(field[0], A_UNDERLINE);   /* Print a line for the option   */
  field_opts_off(field[0], O_AUTOSKIP);    /* Don't go to next field when this */
            /* Field is filled up     */
  set_field_back(field[1], A_UNDERLINE); 
  field_opts_off(field[1], O_AUTOSKIP);

  /* Create the form and post it */
  my_form = new_form(field);
  post_form(my_form);
  refresh();
  
  mvprintw(4, 10, "Value 1:");
  mvprintw(6, 10, "Value 2:");
  refresh();

  /* Loop through to get user requests */
  while((ch = getch()) != KEY_F(1))
  {  switch(ch)
    {  case KEY_DOWN:
        /* Go to next field */
        form_driver(my_form, REQ_NEXT_FIELD);
        /* Go to the end of the present buffer */
        /* Leaves nicely at the last character */
        form_driver(my_form, REQ_END_LINE);
        break;
      case KEY_UP:
        /* Go to previous field */
        form_driver(my_form, REQ_PREV_FIELD);
        form_driver(my_form, REQ_END_LINE);
        break;
      case 127 || 8:
        
      default:
        /* If this is a normal character, it gets */
        /* Printed          */  
        form_driver(my_form, ch);
        break;
    }
  }

  /* Un post form and free the memory */
  unpost_form(my_form);
  free_form(my_form);
  free_field(field[0]);
  free_field(field[1]); 

  endwin();
  return 0;
}
