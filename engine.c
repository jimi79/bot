#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include "engine.h"

const int SIZE_BOARD = 4;

void board_init(board *b) {
	for (int i = 0; i < SIZE_BOARD; i++) {
		for (int j = 0; j < SIZE_BOARD; j++) {
			b->board[i][j] = 0;
		}
	}
}

board *board_new() {
	return malloc(sizeof(board));
}

void board_free(board *b) {
	free(b);
}

void board_print(board *b) {
	int i, j;
	int space = 5;
	char c[9] = "--------";
	for (i = 0; i < SIZE_BOARD; i++) {
		for (j = 0; j < SIZE_BOARD; j++) {
			printw(c);
		}
		printw("-\n");
		int val;
		for (j = 0; j < SIZE_BOARD; j++) {
			val = b->board[i][j];	
			if (val == 0) {
				printw("|       ");
			} else { 
				printw("| %*d ", space, b->board[i][j]);
			}
		}
		printw("|\n");
	} 
	for (j = 0; j < SIZE_BOARD; j++) {
		printw(c);
	}
	printw("-\n");
}

void board_add_random(board *b) {
	b->board[2][2] = 2048;
}
