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
	int s_score = 0;
	int score;
	clock_t time = 0;
	int sub_time = 0;
	int count = 30;
	for (int i = 0; i < count; i++) {
		sub_time = clock();
		score = benchmark_test_bot();
		printf("%d,", score);
		fflush(stdout);
		s_score += score;
		sub_time = clock() - sub_time;
		time += sub_time; 
	}
	*avg_time = (double)time / count;
	*avg_score = (double)s_score / count;
}

void benchmark_test() {
	double time;
	double score;
	benchmark_tests_bot(&time, &score);
	printf("avg clocks run: %0.2f millions, avg score: %0.2f\n", time / 100000, score);
}

void benchmark_compare_method() {
	printf("NONE, 3: ");
	benchmark_init_bot_calc(CALC_NONE);
	bot_max_depth = 3;
	calc_count_cells_to_add = 3;
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
}

void benchmark_compare_depth() {
	for (bot_max_depth = 2;bot_max_depth < 9; bot_max_depth++) { 
		benchmark_init_bot_calc(CALC_NONE);
		printf("NONE, depth %d: ", bot_max_depth);
		benchmark_test();
	}
	for (calc_count_cells_to_add = 1;calc_count_cells_to_add < 4; calc_count_cells_to_add++) { 
		for (bot_max_depth = 2;bot_max_depth < 5; bot_max_depth++) { 
			benchmark_init_bot_calc(CALC_POOR);
			printf("POOR with %d cells, depth %d: ", calc_count_cells_to_add, bot_max_depth);
			benchmark_test();

			benchmark_init_bot_calc(CALC_RANDOM);
			printf("RANDOM with %d, depth %d: ", calc_count_cells_to_add, bot_max_depth);
			benchmark_test(); 
		}
	} 
}

void benchmark_depth_versus_cells() {
	for (int i = 1; i <= 5; i++) {
		bot_max_depth = i;
		calc_count_cells_to_add = 6 - i;
		printf("depth: %d, cells: %d:", bot_max_depth, calc_count_cells_to_add);
		benchmark_init_bot_calc(CALC_POOR);
		benchmark_test();
	}
}

void benchmark_poor_depth() {
	calc_count_cells_to_add = 3;
	benchmark_init_bot_calc(CALC_POOR);
	for (bot_max_depth = 2; bot_max_depth < 6; bot_max_depth++) {
		printf("depth: %d, cells: %d:", bot_max_depth, calc_count_cells_to_add);
		benchmark_test();
	}
}

void main() {
	srand(time(NULL));
	//benchmark_compare_method();
	//benchmark_compare_depth();
	//benchmark_depth_versus_cells();
	benchmark_poor_depth();
}
