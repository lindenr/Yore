/* main.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/rand.h"
#include "include/vision.h"
#include "include/loop.h"
#include "include/panel.h"
#include "include/generate.h"
#include "include/output.h"
#include "include/graphics.h"
#include "include/save.h"
#include "include/magic.h"
#include "include/vector.h"
#include "include/dlevel.h"

#include <stdlib.h>
#include <stdio.h>

void print_intro ()
{
	txt_mvprint (1, 00, "Welcome to Yore v"YORE_VERSION);
	txt_mvprint (2, 10, "* A game guide is not yet in place.");
	txt_mvprint (3, 10, "* A wiki is not yet in place.");
	gr_refresh ();
}

void draw_box_fill (uint32_t yl, uint32_t xl, uint32_t ys, uint32_t xs, uint32_t fill)
{
	int x, y;
	for (x = 1; x < xs; ++x)
		for (y = 1; y < ys; ++y)
			txt_mvaddch (yl + y, xl + x, fill);
	txt_box (yl, xl, ys, xs);
}

bool game_intro ()
{
	int c, by, bx, bh = 9, bw = 50;
	by = (glnumy - bh - 10)/2;
	bx = (glnumx - bw)/2;
	gr_clear ();
	txt_box (by, bx, bh, bw);
	txt_mvprint (by+2, bx+2, "Back in the days of Yore, in a land far removed");
	txt_mvprint (by+3, bx+2, "from our current understanding of the universe,");
	txt_mvprint (by+4, bx+3,  "when magic flowed throughout the air as water");
	txt_mvprint (by+5, bx+3,  "flowed through the sea, and the Gods lived in");
	txt_mvprint (by+6, bx+4,   "harmony with the people; it was a time when");
	txt_mvprint (by+7, bx+6,     "anything and everything was possible...");
	gr_refresh ();
	gr_noecho ();
	gr_tout (666);
	while (1)
	{
		txt_mvprint (by+11, (glnumx - 30)/2, "[hit the spacebar to continue]");
		gr_refresh ();
		do
			c = gr_getch();
		while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			return false;
		txt_mvprint (by+11, (glnumx - 30)/2, "                              ");
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
	dlevel_init ();
	rseed = RNG_get_seed ();
	RNG_main = RNG_INIT (rseed);

	setup_U ();
	atexit (all_things_free);

	if (!game_intro())
		goto quit_game;

	print_intro ();
	mons_gen (cur_dlevel, 0, 15150);

	txt_mvprint (8, 6, "Who are you? ");
	gr_refresh ();

	for (i = 0, *(real_player_name + 1) = '\0';
		 i < 10 && *(real_player_name + 1) == '\0';
		 ++i)
	{
		if (i)
			txt_mvprint (10, 6, "Please type in your name.");
		gr_refresh ();
		gr_move (8, 19);
		gr_getstr (real_player_name + 1, 80);
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

	
	generate_map (cur_dlevel, LEVEL_NORMAL);

	gr_clear ();

	gr_movecam (player->yloc - (glnumy/2), player->xloc - (glnumx/2));

	//if (argc > 1) restore("Yore-savegame.sav");

	p_pane ();
	update_stats();

	do
		main_loop();
	while (U.playing == PLAYER_PLAYING);

  quit_game:
	if (U.playing == PLAYER_LOSTGAME)
		printf("Goodbye %s...\n", pmons.name + 1);
	else if (U.playing == PLAYER_SAVEGAME)
		printf("See you soon...\n");
	else if (U.playing == PLAYER_STARTING)
		printf("Give it a try next time...\n");
	else if (U.playing == PLAYER_WONGAME)
		printf("Congratulations %s...\n", pmons.name + 1);

	exit(0);
}
