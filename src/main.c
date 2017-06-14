/* main.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/rand.h"
#include "include/vision.h"
#include "include/loop.h"
#include "include/panel.h"
#include "include/generate.h"
#include "include/graphics.h"
#include "include/save.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/timer.h"
#include "include/pixel.h"

#include <stdlib.h>
#include <stdio.h>

void print_intro ()
{
//	gra_mvprint (gra, 1,  0, "Welcome to Yore v"YORE_VERSION);
//	gra_mvprint (gra, 2, 10, "* A game guide is not yet in place.");
//	gra_mvprint (gra, 3, 10, "* A wiki is not yet in place.");
}

bool game_intro ()
{
	bool ret = false;
	int c, by, bx, bh = 9, bw = 50;
	by = (txt_h - bh - 10)/2;
	bx = (txt_w - bw)/2;
	Graph ibox = gra_init (bh+12, bw+1, by, bx, bh+12, bw+1);
	//txt_clear ();
	gra_dbox (ibox, 0, 0, bh, bw);
	gra_mvprint (ibox, 2, 2, "Back in the days of Yore, in a land far removed");
	gra_mvprint (ibox, 3, 2, "from our current understanding of the universe,");
	gra_mvprint (ibox, 4, 3,  "when magic flowed throughout the air as water");
	gra_mvprint (ibox, 5, 3,  "flowed through the sea, and the Gods lived in");
	gra_mvprint (ibox, 6, 4,   "harmony with the people; it was a time when");
	gra_mvprint (ibox, 7, 6,     "anything and everything was possible...");
	//txt_echo (0);
	csr_hide ();
	gr_tout (666);
	while (1)
	{
		gra_mvprint (ibox, 11, (bw - 30)/2, "[hit the spacebar to continue]");
		do
			c = gr_getch();
		while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			goto fin;
		gra_mvprint (ibox, 11, (bw - 30)/2, "                              ");
		do
			c = gr_getch();
		while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			goto fin;
	}
	gr_tout (0);
	//txt_echo (1);
	//txt_clear ();
	ret = true;
  fin:
	gra_free (ibox);
	return ret;
}

int main (int argc, char *argv[])
{
	int i;

	//test_do ();
	//return 0;
	gr_onresize = p_init;
	gr_onrefresh = px_showboxes;
	map_graph = gra_init (100, 300, 0, 0, txt_h - PANE_H, txt_w - 1);
	gr_init();

	px_csr ();
	//ru_start (3);
	dlv_init ();
	rng_init ();

	setup_U ();

	if (!game_intro())
		goto quit_game;

	csr_show ();
	print_intro ();
	mons_gen (cur_dlevel, 0, 15150);

	txt_mvprint (8, 6, "Who are you? ");

	for (i = 0, *(real_player_name + 1) = '\0';
		 i < 10 && *(real_player_name + 1) == '\0';
		 ++i)
	{
		if (i)
			txt_mvprint (10, 6, "Please type in your name.");
		txt_move (8, 19);
		txt_getstr (real_player_name + 1, 80);
	}

	if (*(real_player_name + 1) == '\0')
		goto quit_game;


	/* So you really want to play? */
	txt_echo (0);
	txt_clear ();
	csr_hide ();
	get_cinfo ();

	/* If the player entered info correctly, then they should be PLAYER_PLAYING: */
	if (U.playing != PLAYER_PLAYING)
		goto quit_game;

	generate_map (dlv_lvl (1), LEVEL_NORMAL);
	generate_map (dlv_lvl (2), LEVEL_NORMAL);

	//txt_clear ();

	gra_centcam (map_graph, player->yloc, player->xloc);

	//if (argc > 1) restore("Yore-savegame.sav");

	p_pane ();
	draw_map ();
	csr_show ();

	//sp_player_shield ();
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

