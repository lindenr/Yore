#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include "include/all.h"

typedef void (*f_ptr) ();
struct Timer
{
	int time;
	f_ptr callback;
};

void t_interval (int, f_ptr);
void t_idle     ();

#endif /* TIMER_H_INCLUDED */

