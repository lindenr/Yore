#ifndef DLEVEL_H_INCLUDED
#define DLEVEL_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"
#include "include/graphics.h"

struct DLevel
{
	Vector all_things[MAP_TILES];
};

void          dlevel_init ();

extern Vector all_dlevels;
extern int    cur_dlevel;

#endif /* DLEVEL_H_INCLUDED */

