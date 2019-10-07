#include "engine.h"
#include "const.h"
#include "pthread.h"

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

int play_get_value(board *b) {
	int res = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			res = res + b->board[i][j] * 10;
			if (b->board[i][j] == 0) {
				res = res + 128;
			}
		}
	}
	return res;
}

int play_try_move(board *b, int direction, int added_cells) { 
	double val;
	board *b2 = board_new();
	//printf("trying move %d\n", direction);
	board_copy(b, b2);
	if (board_move(b2, direction)) {
		if (!board_full(b2) && (added_cells < max_depth) && (board_get_max(b) >= 16)) {
			val = play_add_cells(b2, added_cells);
		} else {
			val = play_get_value(b2);
		}
	} else { 
		val = play_get_value(b2);
	}
	board_free(b2);
	//printf("trying move %d, value = %0.2f\n", direction, val);
	return val;
}

int play_loop_move(board *b, int added_cells) {
	int best_val = -1;
	int best_dir = -1;
	int val;
	for (int i = 0; i < 4; i++) {
		val = play_try_move(b, i, added_cells);
		if (val > best_val) {
			best_val = val;
			best_dir = i; 
		} 
	} 
	return best_val;
}

double play_add_cells(board *b, int added_cells) { 
	double sum = 0;
	double count = 0;
	int free = free_space(b);
	int ii = 0;
	//int inc = free / 3; 
 	//if (inc == 0) { inc = 1; } // try with random values, but not for all cells
	int inc = 1;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (b->board[i][j] == 0) {
				ii++;
				if (ii % inc == 0) {
					for (int k = 2; k <= 4; k = k + 2) {
						board *b2 = board_new();
						board_copy(b, b2);
						b2->board[i][j] = k;
						b2->last_modified_y = i;
						b2->last_modified_x = j;
						int best_dir;
						double best_score;
						sum = sum + play_loop_move(b2, added_cells + 1); 
						count++;
						board_free(b2);
					}
				}
			}
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
		// test
	//	pthread_join(th[i], NULL); 
	}
	for (int i = 0; i < 4; i++) {
		// test
		pthread_join(th[i], NULL); 
		board_free(th_par[i].b);
	} 
	//printf("%d -> %0.2f\n", th_par[0].direction, th_par[0].value);
	double best_value = th_par[0].value;
	double value;
	int direction = 0;
	for (int i = 1; i < 4; i++) {
		//printf("%d -> %0.2f\n", th_par[i].direction, th_par[i].value);
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
	max_depth = 3; // tree depth
	if (f < 6) {
		max_depth = 4; 
	} 
	dir = play_thread_launcher(b);
	board_move(b, dir);
}

