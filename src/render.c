//
// Created by atuser on 1/27/18.
//

#include <stdlib.h>
#include <signal.h>
#include <autogentoo/render.h>
#include <autogentoo/command_line.h>
#include <ncurses.h>

static char keep_running = 1;
static WINDOW* current_window;
static WindowManager* parent;

WindowManager* init_manager () {
    WindowManager* out = malloc (sizeof (WindowManager));
    out->main = initscr();
    out->subwindows = small_map_new(5, 5);
    out->startpos = small_map_new(5, 5);
    out->keybindings = vector_new(sizeof (KeyBinding), REMOVE | UNORDERED);
    
    return out;
}

void render (WindowManager* _parent) {
    signal(SIGINT, render_finish);
    keypad(stdscr, TRUE);
    nonl();
    parent = _parent;
    
    cbreak();
    noecho();
    current_window = stdscr;
    
    if (has_colors()) {
        start_color ();
        
        init_pair(RED_PAIR, COLOR_RED, COLOR_BLACK);
        init_pair(GREEN_PAIR, COLOR_GREEN, COLOR_BLACK);
        init_pair(YELLOW_PAIR, COLOR_YELLOW, COLOR_BLACK);
        init_pair(BLUE_PAIR, COLOR_BLUE, COLOR_BLACK);
        init_pair(CYAN_PAIR, COLOR_CYAN, COLOR_BLACK);
        init_pair(MAGENTA_PAIR, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(WHITE_PAIR, COLOR_WHITE, COLOR_BLACK);
    }
    
    init_windows (parent);
    current_window = winget ("cmdline");
    
    set_binding(parent, KEY_LEFT, arrows);
    set_binding(parent, KEY_RIGHT, arrows);
    set_binding(parent, KEY_UP, arrows);
    set_binding(parent, KEY_DOWN, arrows);
    set_binding(parent, 127, delete_char);
    set_binding(parent, 'q', stop_running);
    
    for (; keep_running;) {
        int c = wgetch(current_window);
        if (!handle_binding (parent, c)) {
            type_char (c);
        }
        
    }
    
    endwin();
}

void arrows (int c) {
    int y, x;
    getyx (current_window, y, x);
    switch (c) {
        case KEY_LEFT:
            wmove(current_window, y, x - 1);
            break;
        case KEY_RIGHT:
            wmove(current_window, y, x + 1);
            break;
        case KEY_UP:
        case KEY_DOWN:
            break;
    }
}

void stop_running (int c) {
    keep_running = 0;
}

void type_char (int c) {
    int y, x;
    getyx (current_window, y, x);
    wresize (current_window, y, x + 1);
    wrefresh(current_window);
    winsch(current_window, c);
    wmove(current_window, y, x + 1);
}

void delete_char (int c) {
    int y, x;
    getyx (current_window, y, x);
    mvwdelch(current_window, y, x - 1);
    wresize (current_window, y, x - 1);
    wrefresh(current_window);
}

void init_windows (WindowManager* parent) {
    int half_down = LINES / 2;
    int half_over = COLS / 2;
    
    small_map_insert(parent->subwindows, "server", subwin(parent->main, half_down + 6, half_over, 0, 0));
    small_map_insert(parent->subwindows, "host", subwin(parent->main, half_down, half_over, 0, half_over));
    small_map_insert(parent->subwindows, "cmd", subwin(parent->main, half_down - 6, 0, half_down + 6, 0));
    small_map_insert(parent->subwindows, "cmdline", subwin(parent->main, 1, 2, LINES - 2, 15));
    
    keypad(winget("cmdline"),1);
    draw_borders(winget ("cmd"), (WindowBorder) {
            ACS_LTEE,
            ACS_RTEE,
            ACS_LLCORNER,
            ACS_LRCORNER,
            ACS_HLINE,
            ACS_VLINE,
            ACS_HLINE,
            ACS_VLINE
    });
    draw_borders(winget ("server"), (WindowBorder) {
            ACS_ULCORNER,
            ACS_TTEE,
            ACS_VLINE,
            ACS_VLINE,
            ACS_HLINE,
            ACS_VLINE,
            ' ',
            ACS_VLINE
    });
    
    init_cmd_window (parent);
    
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

StartPos* init_startpos (int y, int x) {
    StartPos* out = malloc (sizeof (StartPos));
    out->y = y;
    out->x = x;
    return out;
}
