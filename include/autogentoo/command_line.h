//
// Created by atuser on 1/27/18.
//

#ifndef AUTOGENTOO_COMMAND_LINE_H
#define AUTOGENTOO_COMMAND_LINE_H

void init_cmd_window(WindowManager* parent);

void set_binding(WindowManager* parent, int key, KeyFunc handler);

KeyFunc get_binding(WindowManager* parent, int key);

void remove_binding(WindowManager* parent, int key);

char handle_binding(WindowManager* parent, int key);

#endif //AUTOGENTOO_COMMAND_LINE_H
