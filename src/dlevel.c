/* dlevel.c */

#include "include/dlevel.h"
#include "include/thing.h"

Vector all_dlevels;
int    cur_dlevel;

void dlevel_init ()
{
	int i;
	for (i = 0; i < MAP_TILES; ++ i)
		all_things[i] = v_dinit (sizeof(struct Thing));

	all_dlevels = v_dinit (sizeof(struct DLevel));
	cur_dlevel = 1;
}

