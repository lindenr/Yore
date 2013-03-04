#ifndef DLEVEL_H_INCLUDED
#define DLEVEL_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"
#include "include/graphics.h"

struct DLevel
{
	int level;
	Vector *things;
	Vector mons;
	uint8_t *sq_seen, *sq_attr;
};

void dlv_init ();
void dlv_make (int);
void dlv_set  (int);

Vector *dlv_things (int);
Vector  dlv_mons   (int);
struct DLevel *dlv_lvl (int);

extern Vector all_dlevels;
extern Vector all_ids;
extern int    cur_level;
extern struct DLevel *cur_dlevel;

#endif /* DLEVEL_H_INCLUDED */

