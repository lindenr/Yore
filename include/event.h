#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/all.h"

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

extern Tick curtick;

void ev_loop ();
int  ev_mons_can (MonsID mons, Ev_type ev);

#include "auto/event.qdecl.h"

struct QEv *ev_queue_aux (Tick udelay, union Event ev);
#define ev_queue(tick, ev, ...) ev_queue_ ## ev ((tick), ##__VA_ARGS__)

#endif /* EVENT_H_INCLUDED */

