#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/all.h"

/* Important assumption about behaviour of state associated to an event:
 * currently the event will only be invalidated if an Item or Monster it
 * talks about is affected */

typedef enum
{
#include "auto/event.enum.h"
} EV_TYPE;

union Event
{
	EV_TYPE type;
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

#include "auto/event.qdecl.h"

struct QEv *ev_queue_aux (Tick udelay, union Event ev);
#define ev_queue(tick, ev, ...) ev_queue_ ## ev ((tick), (union Event) { .ev = {EV_ ## ev, __VA_ARGS__}})

#endif /* EVENT_H_INCLUDED */

