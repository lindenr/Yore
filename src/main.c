/* main.c */

#include "include/all.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/monst.h"
#include "include/rand.h"
#include "include/vision.h"
#include "include/loop.h"
#include "include/pline.h"
#include "include/generate.h"
#include "include/thing.h"
#include "include/output.h"
#include "include/util.h"
#include "include/mycurses.h"
#include "include/graphics.h"
#include "include/save.h"
#include "include/magic.h"

void print_intro()
{
	gr_mvprintc(1, 00, "Welcome to Yore v"YORE_VERSION);
	gr_mvprintc(2, 10, "* A game guide is not yet in place.");
	gr_mvprintc(3, 10, "* A wiki is not yet in place.");
	gr_refresh();
}

void draw_box(uint32_t yl, uint32_t xl, uint32_t ys, uint32_t xs)
{
	uint32_t xt = xs - 1, yt = ys - 1;
	mvaddch(yl, xl, ACS_ULCORNER);
	mvaddch(yl + ys, xl, ACS_LLCORNER);
	mvaddch(yl, xl + xs, ACS_URCORNER);
	mvaddch(yl + ys, xl + xs, ACS_LRCORNER);
	while (xt--)
	{
		mvaddch(yl, xl + xt + 1, ACS_HLINE);
		mvaddch(yl + ys, xl + xt + 1, ACS_HLINE);
	}
	while (yt--)
	{
		mvaddch(yl + yt + 1, xl, ACS_VLINE);
		mvaddch(yl + yt + 1, xl + xs, ACS_VLINE);
	}
}

void draw_box_fill(uint32_t yl, uint32_t xl, uint32_t ys, uint32_t xs,
				   uint32_t fill)
{
	int x, y;
	for (x = 1; x < xs; ++x)
		for (y = 1; y < ys; ++y)
			mvaddch(yl + y, xl + x, fill);
	draw_box(yl, xl, ys, xs);
}
/*
bool game_intro()
{
	int c;
	//screenshot();
	//draw_box(5, 15, 9, 50);
	gr_mvprintc(7, 17, "Back in the days of Yore, in a land far removed");
	gr_mvprintc(8, 17, "from our current understanding of the universe,");
	gr_mvprintc(9, 18,  "when magic flowed throughout the air as water");
	gr_mvprintc(10, 18, "flowed through the sea, and the Gods lived in");
	gr_mvprintc(11, 19,  "harmony with the people; it was a time when");
	gr_mvprintc(12, 20,   "anything and everything was possible...");
	refresh();
	noecho();
	in_tout(666);
	while (1)
	{
		gr_mvprintc(16, 25, "[hit the spacebar to continue]");
		do
			c = getch();
		while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			return false;
		gr_mvprintc(16, 25, "                              ");
		do
			c = getch();
		while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			return false;
	}
	in_tout(0);
	echo();
	unscreenshot();
	return true;
}*/ // TODO graphically

int main (int argc, char *argv[])
{
	int i;
	uint32_t rseed;

	gr_init();

	rseed = RNG_get_seed();
	RNG_main = RNG_INIT(rseed);
	magic_init();
	setup_U();
	atexit (all_things_free);

	//if (!game_intro())
	//	goto quit_game;
	
	//init_map();
	print_intro();
	mons_gen(0, 0);

	gr_mvprintc(8, 6, "Who are you? ");
	gr_refresh();

	for (i = 0, *(real_player_name + 1) = '\0';
		 i < 10 && *(real_player_name + 1) == '\0';
		 ++i)
	{
		if (i)
			gr_mvprintc(10, 6, "Please type in your name.");
		gr_refresh();
		gr_move(8, 19);
		gr_getstr(real_player_name + 1);
	}

	if (*(real_player_name + 1) == '\0')
		goto quit_game;

	generate_map(LEVEL_NORMAL);

	/* So you really want to play? */
	gr_noecho();

	gr_clear();
	get_cinfo();

	/* If the player entered info correctly, then they should be PLAYER_PLAYING: */
	if (U.playing != PLAYER_PLAYING)
		goto quit_game;

	gr_clear();
	pline_check();

	//if (argc > 1) restore("Yore-savegame.sav");

	do
	{
		update_stats();
		main_loop();
	}
	while (U.playing == PLAYER_PLAYING);

  quit_game:
	if (U.playing == PLAYER_LOSTGAME)
		printf("Goodbye %s...\n", get_pmonster()->name + 1);
	else if (U.playing == PLAYER_SAVEGAME)
		printf("See you soon...\n");
	else if (U.playing == PLAYER_STARTING)
		printf("Give it a try next time...\n");
	else if (U.playing == PLAYER_WONGAME)
		printf("Congratulations %s...\n", get_pmonster()->name + 1);

	exit(0);
}
