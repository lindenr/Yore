#ifndef LOOP_H_INCLUDED
#define LOOP_H_INCLUDED

#include "mycurses.h"
#include "bool.h"

#define LOOP_AGAIN \
mvaddch(OUT_LOC(monsters_on_level[1].yloc, monsters_on_level[1].xloc),\
        int(mons[monsters_on_level[1].id].ch)|mons[monsters_on_level[1].id].col);\
mvaddch(OUT_LOC(monsters_on_level[0].yloc, monsters_on_level[0].xloc),\
        int(mons[monsters_on_level[0].id].ch)|mons[monsters_on_level[0].id].col);\
refresh();\
move(OUT_LOC(monsters_on_level[1].yloc, monsters_on_level[1].xloc));\
in = getch();
#define END_LOOP return(false)
#define CONTINUE_LOOP return(true)

#define CLEAR_AT(p) mvaddch(p,' ')
#define CLEAR_LINE(y) mvprintw(y,0,LINE_OF_SPACES)

bool     main_loop (void);
char     in;
extern uint64_t Time;

#endif /* LOOP_H_INCLUDED */
