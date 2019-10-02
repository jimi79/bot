#include <string.h>
#include "engine.h"

void bot_board_score(board *b) {
} 

int play_1(board *b, FILE *log) {
	int dir;
	dir = 0;
	if (!board_move(b, 0)) {
		dir = 2;
		if (!board_move(b, 2)) {
			dir = 1;
			if (!board_move(b, 1)) {
				dir = 3;
				if (!board_move(b, 3)) {
					dir = -1;
				}
			}
		} 
	}
	return dir;
}

int get_best_dir(board *b, FILE *log) {
	board *b2;
	int best = -1;
	int best_dir = -1;
	int val;
	int dir; 
	for (int i = 0; i < 4; i++) {
		switch (i) {
			case 0: { dir = 0; } break;
			case 1: { dir = 2; } break;
			case 2: { dir = 1; } break;
			case 3: { dir = 3; } break;
		} 
		b2 = board_new();
		board_copy(b, b2);
		if (board_move(b2, dir)) {
			val = board_get_max(b2);
			fprintf(log, "%d->%d (%d), ", dir, val, best);
			if (val > best) {
				best = val;
				best_dir = dir;
			}
		} 
		board_free(b2);
	}
	fprintf(log, "\n");
	board_move(b, best_dir);
	return best_dir;
}

int play_2(board *b, FILE *log) {
	int best_dir = get_best_dir(b, log);
	return best_dir;
}

int play_3(board *b, FILE *log) {
	// take a board
	// for each dir, try then to add all possible numbers
	// for each, take best dir
	// and after a depth of 10, pull back score
	// and average it
	// how do you average, knowing that there could be 2 or 150 branches? do they all have the same break
}
