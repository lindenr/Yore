/* main.c
 * Linden Ralph */

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

int main (int argc, char *argv[])
{
    char input[100];
    int i;
    uint32_t rseed;

    initscr();
    rseed = RNG_get_seed();
    RNG_main = RNG_INIT(rseed);
    setup_U();

    if (!game_intro()) goto quit_game;

    generate_map(LEVEL_MINES);
    init_map();
    print_intro();
    mvprintw(8, 6, "Who are you? ");
    refresh();

    for (i = 0,    *(real_player_name+1)  = '\0';
         i < 10 && *(real_player_name+1) == '\0';
         ++ i)
    {
        if (i) mvprintw(10, 6, "Please type in your name.");
        refresh();
        move(8, 19);
        getstr(real_player_name+1);
    }

    if (*(real_player_name+1) == '\0') goto quit_game;

    /* So you really want to play? */
    noecho();

    clear_screen();
    get_cinfo();

    /* If the player entered info correctly, then they should be PLAYER_PLAYING: */
    if (U.playing != PLAYER_PLAYING) goto quit_game;

    clear_screen();
    pline_check();

    do
    {
        update_map();
        main_loop();
    }
    while(U.playing == PLAYER_PLAYING);

  quit_game:
    endwin();

    if (U.playing == PLAYER_LOSTGAME)
        printf("Goodbye %s...\n", get_pmonster()->name+1);
    else if (U.playing == PLAYER_SAVEGAME)
        printf("See you soon...\n");
    else if (U.playing == PLAYER_STARTING)
        printf("Give it a try next time...\n");
    else if (U.playing == PLAYER_WONGAME)
        printf("Congrtulations %s...\n", get_pmonster()->name+1);

    exit(0);
}
