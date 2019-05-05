//
// Created by atuser on 1/27/18.
//

#ifndef AUTOGENTOO_RENDER_H
#define AUTOGENTOO_RENDER_H

#include <autogentoo/hacksaw/hacksaw.h>

enum {
	RED_PAIR = 1,
	GREEN_PAIR,
	YELLOW_PAIR,
	BLUE_PAIR,
	CYAN_PAIR,
	MAGENTA_PAIR,
	WHITE_PAIR
};

typedef unsigned chtype;
typedef struct _win_st WINDOW;

typedef struct __WindowManager WindowManager;
typedef struct __WindowBorder WindowBorder;
typedef struct __KeyBinding KeyBinding;
typedef struct __StartPos StartPos;

typedef void (* KeyFunc)(int key);

struct __WindowManager {
	WINDOW* main;
	SmallMap* subwindows;
	Vector* keybindings;
	SmallMap* startpos;
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

struct __StartPos {
	int y;
	int x;
};

struct __KeyBinding {
	int key;
	KeyFunc handler;
};

WindowManager* init_manager();

void render(WindowManager* parent);

void init_windows(WindowManager* parent);

void draw_borders(WINDOW* screen, WindowBorder border);

void window_manager_free(WindowManager* wman);

void render_finish(int sig);

void delete_char(int c);

void stop_running(int c);

void type_char(int c);

void arrows(int c);

StartPos* init_startpos(int y, int x);

#define winget(winname) small_map_get(parent->subwindows, (winname))
#define winstartget(_y, _x) \
{ \
StartPos* t = small_map_get (parent->startpos, small_map_get_key(parent->subwindows, current_window)); \
(_y) = t->y; \
(_x) = t->x; \
}

#endif //AUTOGENTOO_RENDER_H
