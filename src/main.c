/* main.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/rand.h"
#include "include/panel.h"
#include "include/generate.h"
#include "include/graphics.h"
#include "include/event.h"

#include <stdlib.h>
#include <stdio.h>

bool game_intro ()
{
	bool ret = false;
	int c, by, bx, bh = 9, bw = 50;
	by = (gr_h - bh - 10)/2;
	bx = (gr_w - bw)/2;

	Graph ibox = gra_init (bh+12, bw+1, by, bx, bh+12, bw+1);
	gra_dbox (ibox, 0, 0, bh, bw);
	gra_mvprint (ibox, 2, 2, "Back in the days of Yore, in a land far removed");
	gra_mvprint (ibox, 3, 2, "from our current understanding of the universe,");
	gra_mvprint (ibox, 4, 3,  "when magic flowed throughout the air as water");
	gra_mvprint (ibox, 5, 3,  "flowed through the sea, and the Gods lived in");
	gra_mvprint (ibox, 6, 4,   "harmony with the people; it was a time when");
	gra_mvprint (ibox, 7, 6,     "anything and everything was possible...");
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
	ret = true;

  fin:
	gra_free (ibox);
	return ret;
}

int main (int argc, char *argv[])
{
//	int i;

	gr_onresize = p_init;
	map_graph = gra_init (100, 300, 0, 0, gr_h - PANE_H, gr_w - 1);
	map_graph->vis = 0;
	gr_init();

	ev_init ();
	dlv_init ();
	rng_init ();

	setup_U ();

	if (!game_intro())
		goto quit_game;

	int iw = 100, ih = 20;
	int ix = 5, iy = 5;
	Graph introbox = gra_init (ih, iw, ix, iy, ih, iw);
	gra_dbox (introbox, 0, 0, ih-1, iw-1);
	gra_mvprint (introbox, 2,  2, "Welcome to Yore v"YORE_VERSION);
	gra_mvprint (introbox, 3, 10, "* A game guide is not yet in place.");
	gra_mvprint (introbox, 4, 10, "* A wiki is not yet in place.");
	mons_gen (cur_dlevel, 0, 15150);

	gra_mvprint (introbox, 8, 6, "Who are you? ");

	/*for (i = 0, *(real_player_name + 1) = '\0';
		 i < 10 && *(real_player_name + 1) == '\0';
		 ++i)
	{
		if (i)
			gra_mvprint (introbox, 10, 6, "Please type in your name.");
		gra_getstr (introbox, 8, 19, real_player_name + 1, 40);
	}*/
	gr_getch();
	gra_free(introbox);

	//if (*(real_player_name + 1) == '\0')
	//	goto quit_game;


	/* So you really want to play? */
	gra_cshow (map_graph);
	get_cinfo ();

	/* If the player entered info correctly, then they should be PLAYER_PLAYING: */
	if (U.playing != PLAYER_PLAYING)
		goto quit_game;

	generate_map (dlv_lvl (1), LEVEL_NORMAL);
	generate_map (dlv_lvl (2), LEVEL_NORMAL);

	//gra_centcam (map_graph, player->yloc, player->xloc);
	map_graph->vis = 1;

	//if (argc > 1) restore("Yore-savegame.sav");

	//p_pane ();
	//draw_map ();

	ev_queue (1, (union Event) { .mgen = {EV_MGEN}});
	ev_loop ();

  quit_game:
	if (U.playing == PLAYER_LOSTGAME)
		printf("Goodbye %s...\n", "Bertha");
	else if (U.playing == PLAYER_SAVEGAME)
		printf("See you soon...\n");
	else if (U.playing == PLAYER_STARTING)
		printf("Give it a try next time...\n");
	else if (U.playing == PLAYER_WONGAME)
		printf("Congratulations %s...\n", "Wertha");

	exit(0);
}

