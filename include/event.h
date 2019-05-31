#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/all.h"
#include "include/drawing.h"

/* Important assumption about behaviour of state associated to an event:
 * currently the event will only be invalidated if an Item or Monster it
 * talks about is affected */

#include "auto/event.enum.h"

union Event
{
	Ev_type type;
#include "auto/event.union.h"
};

struct QEv
{
	EvID ID;
	Tick tick;
	union Event ev;
};

void ev_loop ();
void ev_debug (int, const char *, const char *);
int  ev_mons_can (MonsID mons, Ev_type ev);
int  qev_lt (struct QEv *q1, struct QEv *q2);

#include "auto/event.qdecl.h"

struct QEv *ev_queue_aux (Tick udelay, union Event ev);
#define ev_queue(tick, ev, ...) \
do {ev_debug (tick, # ev, # __VA_ARGS__); ev_queue_ ## ev ((tick), ##__VA_ARGS__);} while (0)

#endif /* EVENT_H_INCLUDED */

