#ifndef DEF_ENGINE_VAR_H
#define DEF_ENGINE_VAR_H

typedef struct _board board;

struct _board {
	int board[4][4];
	int last_modified_x;
	int last_modified_y; 
	int win_y;
	int win_x;
	char path[10]; 
};

#endif
