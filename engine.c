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

void board_print_ncurses(board *b) {
	int i, j;
	int space = 5;
	char c[9] = "--------";
	move(0, 0);
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
	refresh();
}

int board_add_random(board *b) {
	int size = 0;
	int array[SIZE_BOARD * SIZE_BOARD];
	int i, j;
	for (i = 0; i < SIZE_BOARD; i++) {
		for (j = 0; j < SIZE_BOARD; j++) {
			if (b->board[i][j] == 0) {
				array[size] = i * SIZE_BOARD + j;
				size++;
			}
		}
	}
	if (size > 0) {
		int position = rand() % size;
		position = array[position];
		i = position / SIZE_BOARD;
		j = position % SIZE_BOARD;
		int val = 2 * (rand() % 2 + 1);
		b->board[i][j] = val;
		return 1;
	} else {
		return 0;
	}
}
