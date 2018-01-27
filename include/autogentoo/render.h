//
// Created by atuser on 1/27/18.
//

#ifndef AUTOGENTOO_RENDER_H
#define AUTOGENTOO_RENDER_H

#include <autogentoo/hacksaw/tools.h>

typedef unsigned chtype;
typedef struct _win_st WINDOW;

typedef struct __WindowManager WindowManager;
typedef struct __WindowBorder WindowBorder;

struct __WindowManager {
    WINDOW* main;
    SmallMap* subwindows;
};

struct __WindowBorder {
    chtype top_left;
    chtype top_right;
    chtype bottom_left;
    chtype bottom_right;
    
    chtype top;
    chtype right;
    chtype bottom;
    chtype left;
};

WindowManager* init_manager ();
void render (WindowManager* parent);
void init_windows (WindowManager* parent);
void draw_borders(WINDOW *screen, WindowBorder border);
void window_manager_free (WindowManager* wman);
void render_finish (int sig);

#endif //AUTOGENTOO_RENDER_H
