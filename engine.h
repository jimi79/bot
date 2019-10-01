typedef struct _board board;

struct _board {
	int board[4][4];
	int last_modified_x;
	int last_modified_y; 
	int win_y;
	int win_x;
};

board *board_new();
void board_init(board *b);
void board_free(board *b);
void board_print(board *b, int y, int x);
int board_add(board *b);
int board_move(board *b, int direction);
void board_print_file(FILE *f, board *b);
int board_full(board *b);
int board_win(board *b);
int board_get_max(board *b);
