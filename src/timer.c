/* timer.c */

#include "include/timer.h"
#include "include/vector.h"
#include "include/graphics.h"

Vector times = NULL;

void t_interval (int len, f_ptr callback, void *arg, int flags)
{
	if (times == NULL)
		times = v_dinit (sizeof(struct Timer));
	struct Timer t = {SDL_GetTicks () + len, callback, arg, flags};
	v_push (times, &t);
}

void t_idle ()
{
	if (times == NULL)
		return;
	int i, current = SDL_GetTicks ();
	for (i = 0; i < times->len; ++ i)
	{
		struct Timer *t = v_at (times, i);
		if (t->time >= current)
			continue;
		t->callback (t->arg);
		v_rem (times, i);
		-- i;
	}
}

void t_flush ()
{
	if (times == NULL)
		return;
	int i;
	for (i = 0; i < times->len; ++ i)
	{
		struct Timer *t = v_at (times, i);
		if ((t->flags & TMR_STOP) == 0)
			continue;
		t->callback (t->arg);
		v_rem (times, i);
		-- i;
	}
}

