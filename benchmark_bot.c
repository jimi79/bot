#include "bot_var.h"
#include "bot.h"
#include <time.h>


void benchmark_init_bot_calc(int calc) {
	for (int i = 0; i < 10; i++) {
		bot_calc[i] = calc;
	}
}

int benchmark_test_bot() {
	board *b = board_new();
	board_init(b); 
	while (!board_full(b)) {
		board_add(b); 
		play(b);
	}
	int max = board_get_max(b);
	board_free(b);
	return max;
}

int benchmark_tests_bot(double *avg_time, double *avg_score) {
	int score = 0;
	int time = 0;
	int sub_time = 0;
	int count = 20;
	for (int i = 0; i < count; i++) {
		sub_time = clock();
		score += benchmark_test_bot();
		sub_time = clock() - sub_time;
		time += sub_time; 
	}
	*avg_time = time / count;
	*avg_score = score / count;
}

void benchmark_test() {
	double time;
	double score;
	benchmark_tests_bot(&time, &score);
	printf("clocks run: %0.2f millions, score: %0.2f\n", time / 100000, score);
}

void benchmark_bot() {
	printf("NONE, 3: ");
	benchmark_init_bot_calc(CALC_NONE);
	bot_max_depth = 3;
	benchmark_test();

	printf("POOR, 3: ");
	benchmark_init_bot_calc(CALC_POOR);
	benchmark_test();

	printf("EDGES, 3: ");
	benchmark_init_bot_calc(CALC_EDGES);
	benchmark_test();

	printf("RANDOM, 3: ");
	benchmark_init_bot_calc(CALC_RANDOM);
	benchmark_test();

	printf("ALL, 3: ");
	benchmark_init_bot_calc(CALC_ALL);
	benchmark_test();

	printf("ALL/EDGES/NONE, 3: ");
	benchmark_init_bot_calc(CALC_NONE);
	bot_calc[0] = CALC_ALL;
	bot_calc[1] = CALC_EDGES; 
	benchmark_test();



	//int calc = added_cells < 1 ? CALC_ALL : added_cells < 2 ? CALC_EDGES : CALC_NONE;
	//int calc = added_cells < 2 ? CALC_ALL : CALC_EDGES;

}

void main() {
	srand(stime(NULL));
	benchmark_bot();
}
