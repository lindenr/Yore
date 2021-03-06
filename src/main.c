/* main.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/rand.h"
#include "include/panel.h"
#include "include/generate.h"
#include "include/graphics.h"
#include "include/event.h"
#include "include/save.h"
#include "include/heap.h"
#include "include/player.h"
#include "include/world.h"

#include <stdio.h>
#include <stddef.h>

int game_intro ()
{
	int ret = 0;
	int bh = 9, bw = 50;
	char c;

	Graph ibox = gra_cinit (bh+12, bw+2);
	gra_dbox (ibox, 0, 0, bh, bw);
	gra_mvprint (ibox, 2, 2, "Back in the days of Yore, in a land far removed");
	gra_mvprint (ibox, 3, 2, "from our current understanding of the universe,");
	gra_mvprint (ibox, 4, 2, " when magic flowed throughout the air as water ");
	gra_mvprint (ibox, 5, 2, " flowed through the sea, and the Gods lived in ");
	gra_mvprint (ibox, 6, 2, "  harmony with the people; it was a time when  ");
	gra_mvprint (ibox, 7, 2, "    anything and everything was possible...    ");
	int t = 666;

	while (1)
	{
		gra_mvprint (ibox, 11, (bw - 30)/2, "[hit the spacebar to continue]");
		do
			c = gr_getch_int (t);
		while (c != ' ' && c != GRK_EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			goto fin;
		gra_mvprint (ibox, 11, (bw - 30)/2, "                              ");
		do
			c = gr_getch_int (t);
		while (c != ' ' && c != GRK_EOF && c != 'q' && c != 'Q');
		if (c == ' ')
			break;
		if (c == 'q' || c == 'Q')
			goto fin;
	}
	ret = 1;

  fin:
	gra_free (ibox);
	gr_clear ();
	return ret;
}

void on_quit ()
{
	if (U.playing == PLAYER_STARTING)
		printf("Give it a try next time...\n");
	else
	{
		U.playing = PLAYER_SAVEGAME;
		save (NULL);
	}
	exit (0);
}

//glyph fire_glyph (int);
int main (int argc, char *argv[])
{
	gr_init (640, 1200);
	//gr_init (768, 1366);
	gr_onresize = p_init;
	gr_quit = on_quit;
	
	p_init ();
	rng_init ();
	ityp_init ();
	pl_init ();

	if (argc > 1)
	{
		generate_map (1, LEVEL_TOWN);
		return 0;
	}
	U.playing = PLAYER_STARTING;

	if (!game_intro())
		goto quit_game;

	int iw = 100, ih = 20;
	int ix = 5, iy = 5;
	Graph introbox = gra_init (ih, iw, ix, iy, ih, iw);
	gra_dbox (introbox, 0, 0, ih-1, iw-1);
	gra_mvprint (introbox, 2,  2, "Welcome to Yore v"YORE_VERSION);
	introbox->def = COL_TXT(15, 0, 0);
	gra_mvprint (introbox, 4, 2, "\07 A game guide is not yet in place.");
	introbox->def = COL_TXT(0, 15, 0);
	gra_mvprint (introbox, 5, 2, "\07 A wiki is not yet in place.");
	introbox->def = COL_TXT_DEF;

	gra_mvprint (introbox, 8, 6, "Who are you? ");
	introbox->def = COL_TXT_BRIGHT;
	char *player_name = malloc(41);
	player_name[0] = '\0';
	gra_getstr (introbox, 8, 19, player_name, 40);

	while (!player_name[0])
	{
		gra_mvprint (introbox, 10, 6, "Please type in your name.");
		gra_getstr (introbox, 8, 19, player_name, 40);
	}
	gra_free (introbox);

	if (!player_name[0])
		goto quit_game;

	gr_clear ();

	/* So you really want to play? */
	world_init (player_name); /* or alternatively load save file TODO */
	world.map = grx_init (11, 50, 150, GLH, GLW, -2, -1, 0, 0, gr_ph - PANE_PH, gr_pw - GLW, 7);
	grx_cshow (world.map);

	U.playing = PLAYER_PLAYING;

	//gra_centcam (map_graph, player->yloc, player->xloc);

	//if (argc > 1) restore("Yore-savegame.sav");
	/*for (i = 0; i < 200; ++ i)
		gra_mvaddch (map_graph, 0 + i/20, 0 + i%20, fire_glyph (i));
	gr_frefresh ();
	gr_getch ();*/

	ev_loop ();

  quit_game:
	if (U.playing == PLAYER_LOSTGAME)
		printf("Goodbye %s...\n", world.player_name);
	else if (U.playing == PLAYER_SAVEGAME)
	{
		printf("Saving game for %s...", world.player_name);
		save (NULL);
		printf(" done!\n");
	}
	else if (U.playing == PLAYER_STARTING)
		printf("Give it a try next time...\n");
	else if (U.playing == PLAYER_WONGAME)
		printf("Congratulations %s...\n", world.player_name);

	exit(0);
}

