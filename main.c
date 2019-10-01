#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include "engine.h"

void ncurses_init() {
	initscr();
	cbreak();
	noecho();
}

void ncurses_deinit() {
	endwin();
}

int get_key() {
	int c;
	c = getch();
	return c;
}

int init() {
	srand(time(NULL));
	ncurses_init();
	keypad(stdscr, TRUE); // to make arrow keys recognizable (not sure what it does actually)
}

int deinit() {
	ncurses_deinit();
}

int get_direction() {
	int c = get_key();
	if (c == KEY_LEFT) {
		printw("left");
	} 

}

int main() { 
	init();
	board *b = board_new();
	board_init(b);
	int full;
	full = !board_add(b);
	full = !board_add(b);
	full = !board_add(b);
	full = !board_add(b);
	board_print(b, 0, 0);
	board_move(b, 1);
	board_print(b, 0, 50);
	board_free(b); 
	int c = get_key();
	deinit();
	return 0;
}
