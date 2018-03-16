//
// Created by atuser on 1/27/18.
//


#include <autogentoo/render.h>
#include <autogentoo/command_line.h>
#include <curses.h>
#include <string.h>

void init_cmd_window(WindowManager* parent) {
	int y, x;
	WINDOW* screen = winget ("cmd");
	
	getmaxyx(screen, y, x);
	
	mvwaddch (screen, 0, (x / 2) - 1, ACS_BTEE);
	
	wattron(screen, A_BOLD);
	wattron(screen, COLOR_PAIR(GREEN_PAIR));
	mvwaddstr(screen, y - 2, 2, "autogentoo > ");
	small_map_insert(parent->startpos, "cmd", init_startpos(y - 2, (int) strlen("autogentoo > ") + 2));
	wattroff (screen, A_BOLD);
	wattron(screen, COLOR_PAIR(GREEN_PAIR));
}

void set_binding(WindowManager* parent, int key, KeyFunc handler) {
	int i;
	
	// Find if its already bound
	for (i = 0; i != parent->keybindings->n; i++) {
		KeyBinding* k = (KeyBinding*)vector_get(parent->keybindings, i);
		if (k->key == key) {
			k->handler = handler;
			return;
		}
	}
	
	KeyBinding new_binding = {key, handler};
	vector_add(parent->keybindings, &new_binding);
}

void remove_binding(WindowManager* parent, int key) {
	int i;
	for (i = 0; i != parent->keybindings->n; i++)
		if (((KeyBinding*) vector_get(parent->keybindings, i))->key == key)
			vector_remove(parent->keybindings, i);
}

KeyFunc get_binding(WindowManager* parent, int key) {
	int i;
	for (i = 0; i != parent->keybindings->n; i++) {
		KeyBinding* k = (KeyBinding*)vector_get(parent->keybindings, i);
		if (k->key == key)
			return k->handler;
	}
	
	return NULL;
}

char handle_binding(WindowManager* parent, int key) {
	KeyFunc handler = get_binding(parent, key);
	if (handler) {
		handler(key);
		return 1;
	}
}