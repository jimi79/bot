#ifndef DEF_ENGINE_H
#define DEF_ENGINE_H

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include "engine_var.h"

board *board_new();
void board_init(board *b);
void board_free(board *b);
void board_print_win(board *b, int y, int x);
int board_add(board *b);
int board_move(board *b, int direction);
void board_print_file(FILE *f, board *b);
int board_full(board *b);
int board_win(board *b);
int board_get_max(board *b);
void board_copy(board *src, board *dst);
int board_get_sum(board *b); 
void print_dir(FILE *f, int dir);
void board_print(board *b);

#endif
