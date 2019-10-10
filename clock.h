clock_t start, end;
int cpu_clock[10]; // current time
int cpu_used[10]; // total time
int cpu_count[10]; // count of measurements
#define c_get_value 1
#define c_move 2
#define c_add 3

void init_clocks();
void print_clocks();
void start_clock(int clock_id);
void end_clock(int clock_id);
