#include "engine.h"
#include "const.h"
#include "pthread.h"

void bot_board_score(board *b) {
} 


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
	board_move(b, best_dir);
	return best_dir;
}

int play_2(board *b, FILE *log) {
	int best_dir = get_best_dir(b, log);
	return best_dir;
}

double play_3_add_cells(board *b, FILE *log, int added_cells);

int play_3_get_value(board *b) {
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

void play_3_best_dir(board *b, FILE *log, int *best_dir, double *best_val, int added_cells) { 
	*best_val = -1;
	double val;
	for (int i = 0; i < 4; i++) {
		board *b2 = board_new();
		board_copy(b, b2);
		if (board_move(b2, i)) {
			if ((added_cells < MAX_DEPTH) && (board_get_max(b) >= 16)) {
				val = play_3_add_cells(b2, log, added_cells);
			} else {
				val = play_3_get_value(b2);
			}
			if (val > *best_val) {
				*best_val = val;
				*best_dir = i; 
			} 
		}
		board_free(b2);
	}
}

double play_3_add_cells(board *b, FILE *log, int added_cells) { 
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
						board *b2 = board_new();
						board_copy(b, b2);
						b2->board[i][j] = k;
						b2->last_modified_y = i;
						b2->last_modified_x = j;
						int best_dir;
						double best_score;
						play_3_best_dir(b2, log, &best_dir, &best_score, added_cells + 1); 
						sum = sum + best_score;
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
	FILE *log;
	int best_dir;
	double value;
}; 

pthread_mutex_t mutex_write_res = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_lock(&mutex_write_res);
// pthread_mutex_unlock(&mutex_write_res);

void *play_3_best_dir_thread(void *args) {
	struct t_th_par *th_par = (struct t_th_par *) args;
	int val; 
	if ((!board_full(th_par->b)) && (board_get_max(th_par->b) >= 16)) {
		val = play_3_add_cells(th_par->b, th_par->log, 0);
	} else {
		val = play_3_get_value(th_par->b); // very important
	}
	th_par->value = val;
	pthread_mutex_unlock(&mutex_write_res); 

}

int play_3_best_dir_thread_launcher(board *b, FILE *log) {
	board *bs[4];
	pthread_t th[4];
	bool started[4];
	struct t_th_par th_par[4];
	for (int i = 0; i < 4; i++) { 
		th_par[i].b = board_new(); 
		board_copy(b, th_par[i].b); 
		th_par[i].direction = i;
		th_par[i].log = log;
		th_par[i].value = 0;
		started[i] = false;
		if (board_move(th_par[i].b, i)) {
			started[i] = true;
			fprintf(log, "Creating thread for direction %d\n", i);
			pthread_create(&th[i], NULL, play_3_best_dir_thread, &th_par[i]);
		}
	}
	for (int i = 0; i < 4; i++) {
		if (started[i]) {
			pthread_join(th[i], NULL); 
			board_free(th_par[i].b);
		}
	} 
	double best_value = th_par[0].value;
	double value;
	int direction = 0;
	fprintf(log, "Value is %0.2f for dir %d\n", th_par[0].value, th_par[0].direction);
	for (int i = 1; i < 4; i++) {
		fprintf(log, "Value is %0.2f for dir %d\n", th_par[i].value, th_par[i].direction);
		if (th_par[i].value > best_value) {
			direction = th_par[i].direction;
			best_value = th_par[i].value;
		} 
	}
	fprintf(log, "direction choosen: %d\n", direction);
	fflush(log);
	return direction;
}

int play_3(board *b, FILE *log) {
	int dir;
	double val;
	play_3_best_dir(b, log, &dir, &val, 0);
	dir = play_3_best_dir_thread_launcher(b, log);
	//board_print_file(log, b);
	board_move(b, dir);
	//board_print_file(log, b);
}

