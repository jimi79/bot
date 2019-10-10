#include <stdio.h>
#include <time.h>
#include "clock.h"

void init_clocks() {
	for (int i = 0; i < 10; i++) {
		cpu_used[i] = 0;
		cpu_count[i] = 0;
	}
}

void print_clocks() {
	printf("get_value: %i / %i\n", cpu_used[c_get_value], cpu_count[c_get_value]);
	printf("move: %i / %i\n", cpu_used[c_move], cpu_count[c_move]);
	printf("add: %i / %i\n", cpu_used[c_add], cpu_count[c_add]); 
}

void start_clock(int clock_id) {
	cpu_clock[clock_id] = clock();
}

void end_clock(int clock_id) {
	cpu_used[clock_id] = clock() - cpu_clock[clock_id];
	cpu_count[clock_id] = cpu_count[clock_id] + 1;
}
