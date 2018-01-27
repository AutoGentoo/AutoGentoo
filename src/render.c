//
// Created by atuser on 1/27/18.
//

#include <stdlib.h>
#include <signal.h>
#include "autogentoo/render.h"
#include <curses.h>

static char keep_running = 1;

WindowManager* init_manager () {
    WindowManager* out = malloc (sizeof (WindowManager));
    out->main = initscr();
    out->subwindows = small_map_new(5, 5);
    
    return out;
}

void render (WindowManager* parent) {
    signal(SIGINT, render_finish);
    keypad(stdscr, TRUE);
    nonl();
    
    cbreak();
    echo();
    
    if (has_colors()) {
        start_color ();
        
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }
    
    init_windows (parent);
    
    for (; keep_running;) {
        int c = getch();
        if (c == 'q')
            keep_running = 0;
    }
    
    endwin();
}

void init_windows (WindowManager* parent) {
    int half_down = LINES / 2;
    int half_over = COLS / 2;
    
    small_map_insert(parent->subwindows, "server", subwin(parent->main, half_down, half_over, 0, 0));
    small_map_insert(parent->subwindows, "host", subwin(parent->main, half_down, half_over, 0, half_over));
    small_map_insert(parent->subwindows, "cmd", subwin(parent->main, half_down - 6, 0, half_down + 6, 0));
    
    draw_borders(small_map_get(parent->subwindows, "cmd"), (WindowBorder) {
            ACS_LTEE,
            ACS_RTEE,
            ACS_LLCORNER,
            ACS_LRCORNER,
            ACS_HLINE,
            ACS_VLINE,
            ACS_HLINE,
            ACS_VLINE
    });
    
    refresh();
}

void draw_borders(WINDOW *screen, WindowBorder border) {
    int x, y, i;
    
    getmaxyx(screen, y, x);
    mvwaddch(screen, 0, 0, border.top_left);
    mvwaddch(screen, y - 1, 0, border.bottom_left);
    mvwaddch(screen, 0, x - 1, border.top_right);
    mvwaddch(screen, y - 1, x - 1, border.bottom_right);
    
    for (i = 1; i < (x - 1); i++) {
        mvwaddch(screen, 0, i, border.top);
        mvwaddch(screen, y - 1, i, border.bottom);
    }
    
    for (i = 1; i < (y - 1); i++) {
        mvwaddch(screen, i, 0, border.left);
        mvwaddch(screen, i, x - 1, border.right);
    }
}

void window_manager_free (WindowManager* wman) {
    small_map_free(wman->subwindows, 0);
    free (wman);
}

void render_finish (int sig) {
    keep_running = 0;
}