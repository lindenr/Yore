/* main.c
 * Linden Ralph */

#include "all.h"
#include <stdint.h>
#include <stdlib.h>

#include "monst.h"

#include "rand.h"
#include "vision.h"
#include "loop.h"
#include "pline.h"
#include "generate.h"
#include "thing.h"
#include "output.h"

/* INTRO */

void print_intro()
{
	mvprintw(0, 00, "                      ");
    mvprintw(1, 00, "Welcome to Yore v0.0.04 pre-alpha");
    mvprintw(2, 10, "* A game guide is not yet in place.");
    mvprintw(3, 10, "* A wiki is not yet in place.");
    mvprintw(5, 10, "(The game is not yet in place.)");
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

void game_intro()
{
	screenshot();
	draw_box(5, 15, 9, 50);
	mvprintw(7,  17, "In the days of Yore, in a land far removed from");
	mvprintw(8,  17, "our current understanding of the universe, when");
	mvprintw(9,  17, "magic flowed throughout the air as water flowed");
	mvprintw(10, 17, "through the sea,  and the Gods lived in harmony");
	mvprintw(11, 18,  "with  the people; it was a time when anything");
	mvprintw(12, 25,         "and everything  was possible...");
	refresh();
	noecho();getch();echo();
	unscreenshot();
}

/* END INTRO */
//struct Item i[] = {{0, 0, NULL}, {3, ITEM_BLES, NULL}};
//struct Monster m[] = {{1, 0, 20, 0, 0, 0, 0}, {5,0,10,0," ",0,0,0}};

int main ()
{
    char input[30];
    int I;
    uint32_t rseed;
    initscr();
    rseed = RNG_get_seed();
	RNG_main = RNG_INIT(rseed);

#if defined(DEBUGGING)
	debug_init("debug_out.txt");
#endif


	game_intro();
    generate_map(LEVEL_MINES);
    init_map();
/*	m[0].name = malloc(30);
m[0].name[0] = '_';*/
    print_intro();
    mvprintw(8, 6, "Who are you? ");
	refresh();
    //getstr(m[0].name+1);
    getstr(input);
    noecho();

    /*new_thing(THING_ITEM, 15, 44, &i[0]);
	m->pack.items[0] = &(i[1]);
    for (I = 0; I < 6; ++ I)
    {
        m[0].attr[I] = m[1].attr[I] = 5;
    }
	new_thing(THING_MONS, 15, 45, &m[0]);
	new_thing(THING_MONS, 14, 45, &m[1]);
*/
    update_map();

	pline_check();
    while(main_loop())
    {
        update_map();
    }

/*
    mvprintw(1,60,"     ");
    mvprintw(1,60,"%d", monsters_on_level[1].HP);
    LOOP_AGAIN;
    while (main_loop());*/

/*    endwin(); */
#if defined(DEBUGGING)
	debug_end();
#endif
    endwin();
}
