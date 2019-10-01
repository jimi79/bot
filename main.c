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

char get_key() {
	char c;
	c = getch();
	return c;
}

int init() {
	srand(time(NULL));
	ncurses_init();
}

int deinit() {
	ncurses_deinit();
}


int main() { 
	init();
	board *b = board_new();
	board_init(b);
	int a;
	while (1) {
		a = board_add_random(b);
		board_print_ncurses(b);
		usleep(100);
		if (a == 0) { 
			break;
		}
	}
	board_free(b); 
	char c = get_key();
	//printw("%c", c);
	deinit();
	return 0;
}
