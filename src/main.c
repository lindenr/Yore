/* main.c */

#include "include/all.h"
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
#include "include/graphics.h"
#include "include/save.h"
#include "include/magic.h"
#include "include/vector.h"

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
	gr_mvaddch(yl, xl, ACS_ULCORNER);
	gr_mvaddch(yl + ys, xl, ACS_LLCORNER);
	gr_mvaddch(yl, xl + xs, ACS_URCORNER);
	gr_mvaddch(yl + ys, xl + xs, ACS_LRCORNER);
	while (xt--)
	{
		gr_mvaddch(yl, xl + xt + 1, ACS_HLINE);
		gr_mvaddch(yl + ys, xl + xt + 1, ACS_HLINE);
	}
	while (yt--)
	{
		gr_mvaddch(yl + yt + 1, xl, ACS_VLINE);
		gr_mvaddch(yl + yt + 1, xl + xs, ACS_VLINE);
	}
}

void draw_box_fill(uint32_t yl, uint32_t xl, uint32_t ys, uint32_t xs, uint32_t fill)
{
	int x, y;
	for (x = 1; x < xs; ++x)
		for (y = 1; y < ys; ++y)
			gr_mvaddch(yl + y, xl + x, fill);
	draw_box(yl, xl, ys, xs);
}

bool game_intro()
{
	int c, by, bx, bh = 9, bw = 50;
	by = (glnumy - bh - 10)/2;
	bx = (glnumx - bw)/2;
	gr_clear ();
	draw_box (by, bx, bh, bw);
	gr_mvprintc (by+2, bx+2, "Back in the days of Yore, in a land far removed");
	gr_mvprintc (by+3, bx+2, "from our current understanding of the universe,");
	gr_mvprintc (by+4, bx+3,  "when magic flowed throughout the air as water");
	gr_mvprintc (by+5, bx+3,  "flowed through the sea, and the Gods lived in");
	gr_mvprintc (by+6, bx+4,   "harmony with the people; it was a time when");
	gr_mvprintc (by+7, bx+6,     "anything and everything was possible...");
	gr_refresh ();
	gr_noecho ();
	gr_tout (666);
	while (1)
	{
		gr_mvprintc (by+11, (glnumx - 30)/2, "[hit the spacebar to continue]");
		gr_refresh ();
		do
			c = gr_getch();
		while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			return false;
		gr_mvprintc (by+11, (glnumx - 30)/2, "                              ");
		gr_refresh ();
		do
			c = gr_getch();
		while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			return false;
	}
	gr_tout (0);
	gr_echo ();
	gr_clear ();
	return true;
}

int main (int argc, char *argv[])
{
	int i;
	uint32_t rseed;

	gr_init ();

	rseed = RNG_get_seed ();
	RNG_main = RNG_INIT (rseed);
	setup_U ();
	atexit (all_things_free);

	if (!game_intro())
		goto quit_game;

	//mlines (3, "asdf", "qwer", "zxcv");
	print_intro ();
	mons_gen (0, 0);

	gr_mvprintc (8, 6, "Who are you? ");
	gr_refresh ();

	for (i = 0, *(real_player_name + 1) = '\0';
		 i < 10 && *(real_player_name + 1) == '\0';
		 ++i)
	{
		if (i)
			gr_mvprintc(10, 6, "Please type in your name.");
		gr_refresh();
		gr_move(8, 19);
		gr_getstr(real_player_name + 1, 83);
	}

	if (*(real_player_name + 1) == '\0')
		goto quit_game;


	/* So you really want to play? */
	gr_noecho ();

	gr_clear ();
	get_cinfo ();

	/* If the player entered info correctly, then they should be PLAYER_PLAYING: */
	if (U.playing != PLAYER_PLAYING)
		goto quit_game;

	gr_mode (GMODE);
	
	generate_map (LEVEL_NORMAL);
	gr_clear ();
	pline_check ();

	gr_movecam (get_player()->yloc - (glnumy/2), get_player()->xloc - (glnumx/2));

	//if (argc > 1) restore("Yore-savegame.sav");

	update_stats();

	do
		main_loop();
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
