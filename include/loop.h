#ifndef LOOP_H_INCLUDED
#define LOOP_H_INCLUDED

#include "include/graphics.h"
#include <stdbool.h>

#define CLEAR_AT(p) mvaddch(p,' ')
#define CLEAR_LINE(y) gr_mvprintc(y,0,LINE_OF_SPACES)

void main_loop(void);
// char in;

extern uint64_t Time;

#endif /* LOOP_H_INCLUDED */
