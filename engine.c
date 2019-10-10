#include <time.h>
#include "engine.h"
#include "const.h"
#include "clock.h"

void board_init(board *b) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			b->board[i][j] = 0;
		}
	}
	strncpy(b->path, "", 1);
}

board *board_new() {
	board *r = malloc(sizeof(board));
	r->last_modified_y = -1;
	r->last_modified_x = -1;
	r->win_y = -1;
	r->win_x = -1;
	return r; 
}

void board_copy(board *src, board *dst) {
	memcpy(dst, src, sizeof(board));
	//strncpy(dst->path, "", 1); // empty string: \0
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

void board_print_win(board *b, int y, int x) {
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
			char c[2] = " ";
			if ((i == b->last_modified_y) && (j == b->last_modified_x)) {
				c[0] = '*';
			}
			if (val == 0) {
				fprintf(f, "|      %s", c);
			} else { 
				fprintf(f, "| %*d%s", space, b->board[i][j], c);
			}
		}
		fprintf(f, "|\n");
	} 
	for (j = 0; j < BOARD_SIZE; j++) {
		fprintf(f, c);
	}
	fprintf(f, "-\n");
} 

void board_print(board *b) {
	board_print_file(stdout, b);
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

void board_print_filename(char *c, board *b) {
	FILE *f = fopen(c, "a+");
	board_print_file(f, b);
	fclose(f);
}


int same_value(board *b, int y, int x, int y2, int x2) {
	return ((b->board[y][x] > 1) && (b->board[y][x] == b->board[y2][x2]));
}

int board_move(board *b, int direction) { 
	start_clock(c_move);
	int possible = false;
	b->last_modified_y = -1;
	b->last_modified_x = -1; 
	if (direction == 0) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE - 1; j++) {
				int moved = true;
				while (moved) {
					moved = false;
					for (int k = j; k < BOARD_SIZE - 1; k++) {
						if ((b->board[k][i] == 0) && (b->board[k+1][i] != 0)) {
							possible = true;
							moved = true;
							b->board[k][i] = b->board[k+1][i];
							b->board[k+1][i] = 0;
						}
					}
				}
				if (same_value(b, j, i, j+1, i)) {
					possible = true;
					b->board[j][i] = b->board[j][i] * 2;
					b->board[j+1][i] = 0;
				}
			}
		} 
	}
	if (direction == 1) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = BOARD_SIZE - 1; j > 0; j--) {
				int moved = true;
				while (moved) {
					moved = false;
					for (int k = j; k > 0; k--) {
						if ((b->board[k][i] == 0) && (b->board[k-1][i] != 0)) {
							possible = true;
							moved = true;
							b->board[k][i] = b->board[k-1][i];
							b->board[k-1][i] = 0;
						}
					}
				}
				if (same_value(b, j, i, j-1, i)) {
					possible = true;
					b->board[j][i] = b->board[j][i] * 2;
					b->board[j-1][i] = 0;
				}
			}
		} 
	}
	if (direction == 2) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE - 1; j++) {
				int moved = true;
				while (moved) {
					moved = false;
					for (int k = j; k < BOARD_SIZE - 1; k++) {
						if ((b->board[i][k] == 0) && (b->board[i][k+1] != 0)) {
							possible = true;
							moved = true;
							b->board[i][k] = b->board[i][k+1];
							b->board[i][k+1] = 0;
						}
					}
				}
				if (same_value(b, i, j, i, j+1)) {
					possible = true;
					b->board[i][j] = b->board[i][j] * 2;
					b->board[i][j+1] = 0;
				}
			}
		} 
	} 
	if (direction == 3) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = BOARD_SIZE - 1; j > 0; j--) {
				int moved = true;
				while (moved) {
					moved = false;
					for (int k = j; k > 0; k--) {
						if ((b->board[i][k] == 0) && (b->board[i][k-1] != 0)) {
							possible = true;
							moved = true;
							b->board[i][k] = b->board[i][k-1];
							b->board[i][k-1] = 0;
						}
					}
				}
				if (same_value(b, i, j, i, j-1)) {
					possible = true;
					b->board[i][j] = b->board[i][j] * 2;
					b->board[i][j-1] = 0;
				}
			}
		} 
	} 
	end_clock(c_move);
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

int board_get_sum(board *b) {
	int sum = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			sum += b->board[i][j];
		}
	}
	return sum; 
}

void dir_to_str(int move, char *smove) {
	switch (move)
	{
		case -1: { strncpy(smove, "none", 5); } break;
		case 0: { strncpy(smove, "up", 3); } break;
		case 1: { strncpy(smove, "down", 5); } break;
		case 2: { strncpy(smove, "left", 5); } break;
		case 3: { strncpy(smove, "right", 6); } break;
	}
}

void dir_to_arrow(int move, char *smove) {
	switch (move)
	{
		case -1: { strncpy(smove, "", 1); } break;
		case 0: { strncpy(smove, "↑", sizeof("↑") + 1); } break;
		case 1: { strncpy(smove, "↓", sizeof("↓") + 1); } break;
		case 2: { strncpy(smove, "←", sizeof("←") + 1); } break;
		case 3: { strncpy(smove, "→", sizeof("→") + 1); } break;
	}

}

void print_dir(FILE *f, int dir) {
	char sdir[20];
	dir_to_str(dir, sdir);
	fprintf(f, "direction = %s\n", sdir); 
}
