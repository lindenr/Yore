#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"
#include "include/monst.h"

void event_mhit   (struct Monster *, struct Monster *, uint32_t);
void event_mkill  (struct Monster *, struct Monster *);
void event_mlevel (struct Monster *);
void event_anoise ();

#endif /* EVENT_H_INCLUDED */
