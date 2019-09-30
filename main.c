#include <stdio.h>
#include "engine.h"
#include <ncurses.h>

void ncurses_init() {
	initscr();
	cbreak();
	noecho();
}

void ncurses_free() {
	endwin();
}

char get_key() {
	char c;
	c = getch();
	return c;
}

int main() { 
	ncurses_init();
	board *b = board_new();
	board_init(b);
	board_add_random(b);
	board_print(b);
	board_free(b); 
	char c;
	c = get_key();
	printw("%c", c);
	ncurses_free();
	return 0;
}
