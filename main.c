/* main.c
 * Linden Ralph */

#include "all.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "monst.h"
#include "rand.h"
#include "vision.h"
#include "loop.h"
#include "pline.h"
#include "generate.h"
#include "thing.h"
#include "output.h"

void print_intro()
{
    mvprintw(1, 00, "Welcome to Yore v0.0.5 pre-alpha");
    mvprintw(2, 10,           "* A game guide is not yet in place.");
    mvprintw(3, 10,           "* A wiki is not yet in place.");
    refresh();
}

void draw_box(uint32_t yl, uint32_t xl, uint32_t ys, uint32_t xs)
{
	uint32_t xt = xs-1, yt = ys-1;
	mvaddch(yl, xl, ACS_ULCORNER);
	mvaddch(yl+ys, xl, ACS_LLCORNER);
	mvaddch(yl, xl+xs, ACS_URCORNER);
	mvaddch(yl+ys, xl+xs, ACS_LRCORNER);
	while (xt--)
	{
		mvaddch(yl,    xl+xt+1, ACS_HLINE);
		mvaddch(yl+ys, xl+xt+1, ACS_HLINE);
	}
	while (yt--)
	{
		mvaddch(yl+yt+1, xl, ACS_VLINE);
		mvaddch(yl+yt+1, xl+xs, ACS_VLINE);
	}
}

void draw_box_fill(uint32_t yl, uint32_t xl, uint32_t ys, uint32_t xs, uint32_t fill)
{
	int x, y;
	for (x = 1; x < xs; ++ x) for (y = 1; y < ys; ++ y) mvaddch(yl+y, xl+x, fill);
	draw_box(yl, xl, ys, xs);
}
/*
void sync_for_ch(char c, int ms)
{
    uint32_t cur = (uint32_t)ms;
    uint32_t start = clock();
}*/

bool game_intro()
{
    int c;
	screenshot();
	draw_box(5, 15, 9, 50);
	mvprintw(7,  17, "Back in the days of Yore, in a land far removed");
	mvprintw(8,  17, "from our current understanding of the universe,");
	mvprintw(9,  18,  "when magic flowed throughout the air as water");
	mvprintw(10, 18,  "flowed through the sea, and the Gods lived in");
	mvprintw(11, 19,   "harmony with the people; it was a time when");
	mvprintw(12, 20,     "anything and everything was possible...");
	refresh();
	noecho();
    in_tout(666);
    while(1)
    {
        mvprintw(16, 25, "[hit the spacebar to continue]");
        do c = getch();
        while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
        if (c == ' ') break;
        if (c == 'q' || c == 'Q') return false;
        mvprintw(16, 25, "                              ");
        do c = getch();
        while (c != ' ' && c != EOF && c != 'q' && c != 'Q');
        if (c == ' ') break;
        if (c == 'q' || c == 'Q') return false;
    }
    in_tout(0);
    echo();
	unscreenshot();
    return true;
}

//struct Item i[] = {{0, 0, NULL}, {3, ITEM_BLES, NULL}};
//struct Monster m[] = {{1, 0, 20, 0, 0, 0, 0}, {5,0,10,0," ",0,0,0}};

int main (int argc, char *argv[])
{
    char input[100];
    int I;
    uint32_t rseed;
    initscr();
    rseed = RNG_get_seed();
	RNG_main = RNG_INIT(rseed);
    U.hunger = 100;

#if defined(DEBUGGING)
	debug_init("debug_out.txt");
#endif


	if (!game_intro())
    {
        pline("Exiting...");
        goto quit_game;
    }
    generate_map(LEVEL_MINES);
    init_map();
    print_intro();
    mvprintw(8, 6, "Who are you? ");
	refresh();
    getstr(real_player_name+1);
    noecho();

    full_clear();
    update_map();

	pline_check();
    while(main_loop())
    {
        update_map();
    }

  quit_game:
#if defined(DEBUGGING)
	debug_end();
#endif
    endwin();
    printf("Goodbye %s...\n", ((struct Monster *)U.player->thing)->name+1);
}
