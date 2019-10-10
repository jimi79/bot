#include <time.h>
#include "engine.h"
#include "const.h"
#include "pthread.h"
#include "math.h"
#include "clock.h"

//#define debug

#define CALC_EDGES 1
#define CALC_ALL 2
#define CALC_WRONG 3
#define CALC_CORNERS 4






int max_depth;

int free_space(board *b) {
	int res = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (b->board[i][j] == 0) {
				res++;
			}
		}
	}
	return res;
} 

double play_add_cells(board *b, int added_cells);

double play_get_value(board *b) {

	start_clock(c_get_value);
	int res = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			res = res + (b->board[i][j] == 0 ? 128 * 128 : b->board[i][j] * b->board[i][j]);
		}
	}
	end = clock();

	end_clock(c_get_value);

	return res;
}

void print_state(board *b, double val, int added_cells) {
	printf("Level: %d\n", added_cells);
	printf("Path: %s\n", b->path);
	printf("Board:\n");
	board_print_file(stdout, b);
	printf("score: %0.2f\n", val);
}

double play_try_move(board *b, int direction, int added_cells) { 
	b->path[added_cells] = "^v<>"[direction];
	b->path[added_cells + 1] = '\0';
	double val;
	board *b2 = board_new();
	//printf("trying move %d\n", direction);
	board_copy(b, b2);
	if (board_move(b2, direction)) {
		if ((added_cells < max_depth) && (board_get_max(b) >= 16)) {
			val = play_add_cells(b2, added_cells);
		} else {
			val = play_get_value(b2);
		}
	} else { 
		val = play_get_value(b2);
	}
#if defined debug
	print_state(b2, val, added_cells);
#endif
	board_free(b2);
	//printf("trying move %d, value = %0.2f\n", direction, val);
	return val;
}

double play_loop_move(board *b, int added_cells) {
	double best_val = -1;
	int best_dir = -1;
	double val;
	for (int i = 0; i < 4; i++) {
		val = play_try_move(b, i, added_cells);
		if (val > best_val) {
			best_val = val;
			best_dir = i; 
		} 
	} 
	return best_val;
}


double play_add_cell(board *b, int added_cells, int y, int x, int cell_value) {
	board *b2 = board_new();
	board_copy(b, b2);
	b2->board[y][x] = cell_value;
	b2->last_modified_y = y;
	b2->last_modified_x = x;
	int best_dir;
	double best_score;
	double board_value = play_loop_move(b2, added_cells + 1); 
	board_free(b2);
	return board_value;

}

double play_add_cells(board *b, int added_cells) { 
	double sum = 0;
	int count = 0; 
	//int calc = added_cells < 2 ? CALC_ALL : CALC_CORNERS;
	int calc = CALC_OLD; 
	int x, y; 
	if (calc == CALC_OLD) {
		double sum = 0;
		double count = 0;
		int free = free_space(b);
		int ii = 0;
		int inc = free / 2; 
		if (inc == 0) { inc = 1; }
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (b->board[i][j] == 0) {
					ii++;
					if (ii % inc == 0) {
						for (int k = 2; k <= 4; k = k + 2) {
							sum = sum + play_add_cell(b, i, j, k);
							count++;
						}
					}
				}
			}
		} 
	} 
	if (calc == CALC_EDGES) {
#ifdef debug
		printf("edges\n");
#endif 
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
			x = i % BOARD_SIZE;
			y = i / BOARD_SIZE;
			if (b->board[y][x] == 0) {
				sum = sum + play_add_cell(b, added_cells, y, x, 2) + play_add_cell(b, added_cells, y, x, 4); 
				count = count + 2;
			}
		} 
		if (count == 0) {
			for (int i = 1; i < BOARD_SIZE - 1; i++) {
				for (int j = 1; j < BOARD_SIZE - 1; j++) {
					if (b->board[i][j] == 0) {
						sum = sum + play_add_cell(b, added_cells, i, j, 2) + play_add_cell(b, added_cells, i, j, 4); 
						count = count + 2;
					}
				}
			} 
		}
	}
	if (calc == CALC_ALL) {
		int j;
		int val;
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE * 2; i++) {
			val = (i % 2 + 1) * 2;
			j = i>>1;
			x = j % BOARD_SIZE;
			y = j / BOARD_SIZE;
			if (b->board[y][x] != 0) {
				sum = sum + play_add_cell(b, added_cells, y, x, val); 
				count++;
			}
		} 
	}
	if (calc == CALC_WRONG) {
		board *b2 = board_new();
		board_copy(b, b2);
		int j;
		int val;
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
			x = i % BOARD_SIZE;
			y = i / BOARD_SIZE;
			if (b2->board[y][x] == 0) {
				b2->board[y][x] = 1;
			}
		} 
		sum = sum + play_loop_move(b2, added_cells + 1); 
		count++;
		board_free(b2);
	}
	if (calc == CALC_CORNERS) {
		for (int i = 1; i < BOARD_SIZE - 1; i++) {
			for (int j = 1; j < BOARD_SIZE - 1; j++) {
				if (b->board[i][j] == 0) {
					sum = sum + play_add_cell(b, added_cells, i, j, 2) + play_add_cell(b, added_cells, i, j, 4); 
					count = count + 2;
				} 
			}
		}
		if (b->board[0][0] == 0) {
			sum = sum + play_add_cell(b, added_cells, 0, 0, 2) + play_add_cell(b, added_cells, 0, 0, 4); 
			count = count + 2;
		}
		if (b->board[0][BOARD_SIZE] == 0) { 
			sum = sum + play_add_cell(b, added_cells, 0, BOARD_SIZE, 2) + play_add_cell(b, added_cells, 0, BOARD_SIZE, 4); 
			count = count + 2;
		}
		if (b->board[BOARD_SIZE][0] == 0) { 
			sum = sum + play_add_cell(b, added_cells, BOARD_SIZE, 0, 2) + play_add_cell(b, added_cells, BOARD_SIZE, 0, 4); 
			count = count + 2;
		}
		if (b->board[BOARD_SIZE][BOARD_SIZE] == 0) { 
			sum = sum + play_add_cell(b, added_cells, BOARD_SIZE, BOARD_SIZE, 2) + play_add_cell(b, added_cells, BOARD_SIZE, BOARD_SIZE, 4); 
			count = count + 2;
		}

	}


	if (count != 0) {
		return (sum / count);
	} else {
		return 0;
	} 
}
 
struct t_th_par {
	board *b; // board to solve
	int direction; 
	double value;
}; 

pthread_mutex_t mutex_write_res = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_lock(&mutex_write_res);
// pthread_mutex_unlock(&mutex_write_res);

void *play_thread(void *args) {
	struct t_th_par *th_par = (struct t_th_par *) args;
	th_par->value = play_try_move(th_par->b, th_par->direction, 0);
}

int play_thread_launcher(board *b) {
	board *bs[4];
	pthread_t th[4];
	bool started[4];
	struct t_th_par th_par[4];
	for (int i = 0; i < 4; i++) { 
		th_par[i].b = board_new(); 
		board_copy(b, th_par[i].b); 
		th_par[i].direction = i;
		th_par[i].value = 0;
		pthread_create(&th[i], NULL, play_thread, &th_par[i]);
#ifdef debug
		pthread_join(th[i], NULL); 
#endif
	}
	for (int i = 0; i < 4; i++) {
#ifndef debug
		pthread_join(th[i], NULL); 
#endif
		board_free(th_par[i].b);
	} 
	double best_value = th_par[0].value;
	double value;
	int direction = 0;
	for (int i = 1; i < 4; i++) {
		if (th_par[i].value > best_value) {
			direction = th_par[i].direction;
			best_value = th_par[i].value;
		} 
	}
	return direction;
}

int play(board *b) {
	int dir;
	double val;
	int f = free_space(b);
	max_depth = 4;
	dir = play_thread_launcher(b);
	board_move(b, dir);
	strncpy(b->path, "", 1);
	return dir;
}

// store history on board, and display all possible outcomes in a file, one per turn
// with a way shorter depth
