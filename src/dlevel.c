/* dlevel.c */

#include "include/dlevel.h"
#include "include/thing.h"

Vector all_ids;
Vector all_dlevels;
int    cur_level;
struct DLevel *cur_dlevel;

void dlv_init ()
{
	all_dlevels = v_dinit (sizeof(struct DLevel));

	void *t = NULL;
	all_ids = v_dinit (sizeof(void *));
	v_push (all_ids, &t);

	dlv_make (1);
	dlv_set (1);
}

void dlv_make (int level)
{
	int i;
	struct DLevel new_level = {level, NULL, v_dinit (sizeof(int))};
	new_level.things = malloc (sizeof(Vector) * MAP_TILES);
	for (i = 0; i < MAP_TILES; ++ i)
		new_level.things[i] = v_dinit (sizeof(struct Thing));
	v_push (all_dlevels, &new_level);
}

void dlv_set (int level)
{
	int i;
	struct DLevel *lvl;
	cur_level = level;
	for (i = 0; i < all_dlevels->len; ++ i)
	{
		lvl = v_at (all_dlevels, i);
		if (lvl->level == level)
			break;
	}
	if (i >= all_dlevels->len)
		return;
	cur_dlevel = lvl;
}

Vector *dlv_things (int level)
{
	int i;
	struct DLevel *lvl;
	for (i = 0; i < all_dlevels->len; ++ i)
	{
		lvl = v_at (all_dlevels, i);
		if (lvl->level == level)
			break;
	}
	if (i >= all_dlevels->len)
		return NULL;
	return lvl->things;
}

Vector dlv_mons (int level)
{
	int i;
	struct DLevel *lvl;
	for (i = 0; i < all_dlevels->len; ++ i)
	{
		lvl = v_at (all_dlevels, i);
		if (lvl->level == level)
			break;
	}
	if (i >= all_dlevels->len)
		return NULL;
	return lvl->mons;
}

struct DLevel *dlv_lvl (int level)
{
	int i;
	struct DLevel *lvl;
	for (i = 0; i < all_dlevels->len; ++ i)
	{
		lvl = v_at (all_dlevels, i);
		if (lvl->level == level)
			break;
	}
	if (i >= all_dlevels->len)
		return NULL;
	return lvl;
}

