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

/* Output of the bresenham algorithm (drawing.c): 0 if we can't see it (outside 
 * our field of vision); 1 if we remember it; and 2 if we are looking at it.
 * The options are mutually exclusive. */
	uint8_t *seen;

/* At any given point: 0 if we can't see past that square (e.g. wall); 1 if we 
 * can remember it (e.g. sword); and 2 if we can't remember its position (e.g. 
 * monster). The options are mutually exclusive. */
	uint8_t *attr;

/* What to see instead if it turns out that we can't remember something */
	glyph *unseen;

	int uplevel, dnlevel;
};

void dlv_init ();
void dlv_make (int, int, int);
void dlv_set  (int);

struct DLevel *dlv_lvl (int);
Vector  *dlv_things (int);
Vector   dlv_mons   (int);
uint8_t *dlv_attr   (int);
int      dlv_dn     (int);
int      dlv_up     (int);

extern Vector all_dlevels;
extern Vector all_ids;
extern int    cur_level;
extern struct DLevel *cur_dlevel;

#endif /* DLEVEL_H_INCLUDED */

