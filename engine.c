#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include "engine.h"

const int BOARD_SIZE = 4;
const int BOARD_WIN = 2048;

void board_init(board *b) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			b->board[i][j] = 0;
		}
	}
}

board *board_new() {
	board *r = malloc(sizeof(board));
	r->last_modified_y = -1;
	r->last_modified_x = -1;
	r->win_y = -1;
	r->win_x = -1;
	return r; 
}

void board_free(board *b) {
	free(b);
}

int board_win(board *b) {
	int win = false;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			win = (b->board[i][j] == BOARD_WIN);
			if (win) { 
				b->win_y = i;	
				b->win_x = j; 
				break;
			}
		}
		if (win) { break; }
	}
	return win;
}

void board_print(board *b, int y, int x) {
	int i, j;
	int space = 5;
	char c[9] = "--------";
	for (i = 0; i < BOARD_SIZE; i++) {
		move(y + i * 2, x);
		for (j = 0; j < BOARD_SIZE; j++) {
			printw(c);
		}
		printw("-");
		move(y + i * 2 + 1, x);
		int val;
		for (j = 0; j < BOARD_SIZE; j++) {
			val = b->board[i][j];	
			if (val == 0) {
				printw("|       ");
			} else { 
				printw("| ");

				int color = 0; 
				if ((i == b->last_modified_y) && (j == b->last_modified_x)) {
					color = 1;
				}
				if ((i == b->win_y) && (j == b->win_x)) {
					color = 2;
				} 
				if (color != 0) { 
					attron(COLOR_PAIR(color));
				}
				printw("%*d", space, b->board[i][j]);
				if (color != 0) { 
					attroff(COLOR_PAIR(1));
				}
				printw(" ");
			}
		}
		printw("|");
	} 
	move(y + BOARD_SIZE * 2, x);
	for (j = 0; j < BOARD_SIZE; j++) {
		printw(c);
	}
	printw("-");
	refresh();
}

void board_print_file(FILE *f, board *b) {
	int i, j;
	int space = 5;
	char c[9] = "--------";
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			fprintf(f, c);
		}
		fprintf(f, "-\n");
		int val;
		for (j = 0; j < BOARD_SIZE; j++) {
			val = b->board[i][j];	
			if (val == 0) {
				fprintf(f, "|       ");
			} else { 
				fprintf(f, "| %*d ", space, b->board[i][j]);
			}
		}
		fprintf(f, "|\n");
	} 
	for (j = 0; j < BOARD_SIZE; j++) {
		fprintf(f, c);
	}
	fprintf(f, "-\n");
} 

int board_add(board *b) {
	int size = 0;
	int array[BOARD_SIZE * BOARD_SIZE];
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (b->board[i][j] == 0) {
				array[size] = i * BOARD_SIZE + j;
				size++;
			}
		}
	}
	if (size > 0) {
		int position = rand() % size;
		position = array[position];
		i = position / BOARD_SIZE;
		j = position % BOARD_SIZE;
		int val = 2 * (rand() % 2 + 1);
		b->board[i][j] = val;
		b->last_modified_y = i;
		b->last_modified_x = j;
		return 1;
	} else {
		return 0;
	}
}

int board_full(board *b) {
	int full = true;
	for (int i = 0; i < 4; i++) {
		if (!full) { break; }
		for (int j = 0; j < 4; j++) {
			full = b->board[i][j] != 0;
			if (!full) { break; }
		}
	}
	return full;

}

int board_move_cell(board *b, int sy, int sx, int dy, int dx) {
	int res;
	b->last_modified_y = -1;
	b->last_modified_x = -1;
	if (b->board[sy][sx] == 0) {
		res = false;
	} else {
		if (b->board[dy][dx] == 0) {
			b->board[dy][dx] = b->board[sy][sx];
			b->board[sy][sx] = 0;
			res = 1;
		} else {
			if (b->board[dy][dx] == b->board[sy][sx]) {
				b->board[dy][dx] = b->board[dy][dx] * 2;
				b->board[sy][sx] = 0;
				res = 1;
			} else {
				res = 0;
			}
		}
	}
	return res;
}

int board_move(board *b, int direction) {
	// 0: up, 1: down, 2: left, 3: right
	int ok = true;
	int possible = false;
	if (direction == 0) {
		for (int i = 1; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				ok = true;
				for (int k = i; k > 0; k--) {
					ok = board_move_cell(b, k, j, k - 1, j);
					possible = possible || ok;
				} 
			}
		}
	}
	if (direction == 1) {
		for (int i = BOARD_SIZE - 2; i >= 0; i--) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				ok = true;
				for (int k = i; k < BOARD_SIZE - 1; k++) {
					ok = board_move_cell(b, k, j, k + 1, j);
					possible = possible || ok;
				} 
			}
		}
	}
	if (direction == 2) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 1; j < BOARD_SIZE; j++) {
				ok = true;
				for (int k = j; k >= 1; k--) {
					ok = board_move_cell(b, i, k, i, k - 1);
					possible = possible || ok;
				} 
			}
		}
	}
	if (direction == 3) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = BOARD_SIZE - 1; j >= 0; j--) {
				ok = true;
				for (int k = j; k < BOARD_SIZE - 1; k++) {
					ok = board_move_cell(b, i, k, i, k + 1);
					possible = possible || ok;
				} 
			}
		}
	} 
	return possible;
}

int board_get_max(board *b) {
	int max = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (b->board[i][j] > max) {
				max = b->board[i][j];
			}
		}
	}
	return max;
	
}
