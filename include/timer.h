#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include "include/all.h"

#define TMR_NONE 0
#define TMR_STOP 1

typedef void (*f_ptr) ();
struct Timer
{
	int time;
	f_ptr callback;
	int flags;
};

void t_interval (int, f_ptr, int flags);
void t_idle     ();
void t_flush    ();

#endif /* TIMER_H_INCLUDED */

