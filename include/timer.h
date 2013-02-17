#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

struct Timer
{
	int time;
	void (*callback) (int);
	int arg;
};

void t_interval (int, void (*) (int), int);
void t_idle     ();

#endif /* TIMER_H_INCLUDED */

