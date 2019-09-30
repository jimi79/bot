struct _board {
	int board[4][4];
};

typedef struct _board board;

board *board_new();
void board_init(board *b);
void board_free(board *b);
void board_print(board *b);
void board_add_random(board *b);
