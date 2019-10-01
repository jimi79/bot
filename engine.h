typedef struct _board board;

struct _board {
	int board[4][4];
};

board *board_new();
void board_init(board *b);
void board_free(board *b);
void board_print_ncurses(board *b);
int board_add_random(board *b);
