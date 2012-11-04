/* dlevel.c */

#include "include/dlevel.h"

Vector all_dlevels;
int    cur_dlevel;

void dlevel_init ()
{
	all_dlevels = v_dinit (sizeof(struct DLevel));
	cur_dlevel = 1;
}

