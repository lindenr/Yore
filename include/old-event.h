#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"
#include "include/monst.h"

void event_mhit   (struct Thing *, struct Thing *, uint32_t);
void event_mkill  (struct Thing *, struct Thing *);
void event_mlevel (struct Thing *);
void event_anoise ();

#endif /* EVENT_H_INCLUDED */
