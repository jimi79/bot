#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include "engine.h"
#include "bot.h"

void write_log(char *s) {
	FILE *f;
	f = fopen("log.log", "a+");
	fprintf(f, s);
	fclose(f);
}


void ncurses_init() {
	initscr();
	cbreak();
	noecho();
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	curs_set(0); // hide cursor
	keypad(stdscr, TRUE); // to make arrow keys recognizable (not sure what it does actually)
}

void ncurses_deinit() {
	endwin();
}

int get_key() {
	int c;
	c = getch();
	return c;
}

int wait_key(int *quit) {
	*quit = false;
	int move = -1;
	while ((move == -1) && (!*quit)) {
		int c = get_key();

		// clear last status
		move(10, 0);
		clrtoeol();

		if (c == KEY_UP) {
			move = 0;
		} 
		if (c == KEY_DOWN) {
			move = 1;
		} 

		if (c == KEY_LEFT) {
			move = 2;
		} 

		if (c == KEY_RIGHT) {
			move = 3;
		} 
		if (c == 'q') {
			*quit = true;
		}
	}
	return move;
}

void user_move(board *b, int *quit) {
	*quit = false;
	int ok = false;
	int move_;
	while (!ok) {
		write_log("1");
		move_ = wait_key(quit);	
		if (*quit) { 
			break;
		}
		ok = board_move(b, move_); 
		if (!ok) {
			move(10, 0);
			printw("not possible");
		}
	}
}

int human() { 
	srand(time(NULL));
	ncurses_init();
	board *b = board_new();
	board_init(b);
	int full; 
	int quit = false;
	while ((!board_full(b)) && (!quit) && (!board_win(b))) {
		board_add(b);
		board_print_win(b, 0, 0);
		user_move(b, &quit);
	} 
	if (board_win(b)) {
		board_print_win(b, 0, 0);
		move(10, 0);
		printw("won, press a key to exit\n");
		getch();
	}

	ncurses_deinit();
	if (quit) {
		printf("bye\n");
	}
	return 0;
}

int bot(int ncurses) {
	srand(time(NULL));
	if (ncurses) {
		ncurses_init();
	}
	board *b = board_new();
	board_init(b);
	int ok = true;
	int dir;
	char smove[4][6];
	/*strncpy(smove[0], "↑", sizeof("↑" + 1));
	strncpy(smove[1], "↓", sizeof("↓" + 1));
	strncpy(smove[2], "←", sizeof("←" + 1));
	strncpy(smove[3], "→", sizeof("→" + 1)); */ // not working in ncurses, check why

	strncpy(smove[0], "^", sizeof("^" + 1));
	strncpy(smove[1], "v", sizeof("v" + 1));
	strncpy(smove[2], "<", sizeof("<" + 1));
	strncpy(smove[3], ">", sizeof(">" + 1));

	while (!board_full(b)) {
		board_add(b);
		if (ncurses) {
			board_print_win(b, 0, 0);
		} else {
			board_print_file(stdout, b); 
			fflush(stdout);
		}
		dir = play(b);
		if (ncurses) {
			move(11,0);
			printw("%s", smove[dir]); 
			refresh(); 
		} else { 
			fprintf(stdout, "direction: %s\n", smove[dir]); 
		}
	}
	if (ncurses) {
		move(12,0);
		printw("max = %d\n", board_get_max(b));
		getch();
		ncurses_deinit();
	}
	board_print_file(stdout, b);
	printf("max = %d\n", board_get_max(b));
}	

int test_board(board *b) {
	ncurses_init();
	srand(time(NULL));
	board *b2 = board_new();
	board *b3 = board_new();
	board *b4 = board_new();
	board *b5 = board_new();
	board_copy(b, b2);
	board_copy(b, b3);
	board_copy(b, b4);
	board_copy(b, b5);

	board_move(b2, 0);
	board_move(b3, 1);
	board_move(b4, 2);
	board_move(b5, 3);
	board_print_win(b, 15, 50);
	board_print_win(b2, 0, 50); 
	board_print_win(b3, 30, 50); 
	board_print_win(b4, 15, 0); 
	board_print_win(b5, 15, 100); 
	board_free(b5);
	board_free(b4);
	board_free(b3);
	board_free(b2);
	getch();
	ncurses_deinit();
}

int test_board_1() {
	board *b = board_new();
	b->board[0][0] = 0;
	b->board[0][1] = 0;
	b->board[0][2] = 0;
	b->board[0][3] = 0;
	b->board[1][0] = 0;
	b->board[1][1] = 0;
	b->board[1][2] = 0;
	b->board[1][3] = 0;
	b->board[2][0] = 0;
	b->board[2][1] = 0;
	b->board[2][2] = 2;
	b->board[2][3] = 0;
	b->board[3][0] = 0;
	b->board[3][1] = 0;
	b->board[3][2] = 0;
	b->board[3][3] = 0;
	test_board(b);
	board_free(b);
}

int test_board_2() {
	board *b = board_new();
	b->board[0][0] = 4;
	b->board[0][1] = 4;
	b->board[0][2] = 4;
	b->board[0][3] = 4;
	b->board[1][0] = 4;
	b->board[1][1] = 4;
	b->board[1][2] = 4;
	b->board[1][3] = 4;
	b->board[2][0] = 4;
	b->board[2][1] = 4;
	b->board[2][2] = 4;
	b->board[2][3] = 4;
	b->board[3][0] = 4;
	b->board[3][1] = 4;
	b->board[3][2] = 4;
	b->board[3][3] = 4;
	test_board(b);
	board_free(b);
}

int test_bot() {
	board *b = board_new();
	board_init(b);
	FILE *log = fopen("test", "w");

	b->board[0][0] = 4;
	b->board[0][1] = 0;
	b->board[0][2] = 2;
	b->board[0][3] = 0;
	b->board[1][0] = 4;
	b->board[1][1] = 0;
	b->board[1][2] = 0;
	b->board[1][3] = 2;
	b->board[2][0] = 0;
	b->board[2][1] = 0;
	b->board[2][2] = 2;
	b->board[2][3] = 0;
	b->board[3][0] = 2;
	b->board[3][1] = 2;
	b->board[3][2] = 2;
	b->board[3][3] = 2;

	play(b);
	fclose(log); 
}

int test() {
	board *b = board_new();

/*
---------------------------------
|     8 |    16 |     4 |     2*|
---------------------------------
|     4 |   512 |    64 |     8 |
---------------------------------
|    64 |  2048 |   128 |    32 |
---------------------------------
|    16 |   256 |     4 |     4 |
---------------------------------
*/
	b->board[0][0] = 16;
	b->board[0][1] = 16;
	b->board[0][2] = 16;
	b->board[0][3] = 16;
	b->board[1][0] = 16;
	b->board[1][1] = 16;
	b->board[1][2] = 16;
	b->board[1][3] = 16;
	b->board[2][0] = 16;
	b->board[2][1] = 16;
	b->board[2][2] = 16;
	b->board[2][3] = 16;
	b->board[3][0] = 0;
	b->board[3][1] = 0;
	b->board[3][2] = 0;
	b->board[3][3] = 0;

	
	board_print_file(stdout, b);
	int dir = play(b);
		
	board_print_file(stdout, b);
	board_free(b);

}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		bot(false);
		//test();
	} else {
		if (!strcmp("bot", argv[1])) { bot(true); }
		if (!strcmp("test", argv[1])) { test(); }
		if (!strcmp("human", argv[1])) { human(); }
	}
}
