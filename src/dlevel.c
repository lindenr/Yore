/* dlevel.c */

#include "include/dlevel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/magic.h"

Vector all_ids;
Vector all_dlevels;
int    cur_level;
struct DLevel *cur_dlevel;

void dlv_init ()
{
	sp_init ();
	sp_list = v_dinit (sizeof(union Spell));
	all_dlevels = v_dinit (sizeof(struct DLevel));

	void *t = NULL;
	all_ids = v_dinit (sizeof(void *));
	v_push (all_ids, &t);

	dlv_make (1, 0, 0);
	dlv_make (2, 1, 0);
	dlv_set (1);
}

void dlv_make (int level, int uplevel, int dnlevel)
{
	int i;
	struct DLevel new_level = {
		level,
		NULL,
		v_dinit (sizeof(int)),
		malloc (sizeof(uint8_t)*MAP_TILES),
		malloc (sizeof(uint8_t)*MAP_TILES),
		malloc (sizeof(glyph)*MAP_TILES),
		uplevel,
		dnlevel
	};
	struct DLevel *lvl = dlv_lvl (uplevel);
	if (lvl)
		lvl->dnlevel = level;
	lvl = dlv_lvl (dnlevel);
	if (lvl)
		lvl->uplevel = level;
	new_level.things = malloc (sizeof(Vector) * MAP_TILES);
	for (i = 0; i < MAP_TILES; ++ i)
		new_level.things[i] = v_dinit (sizeof(struct Thing));
	memset (new_level.seen, 0, sizeof(uint8_t)*MAP_TILES);
	memset (new_level.attr, 0, sizeof(uint8_t)*MAP_TILES);
	memset (new_level.unseen, 0, sizeof(glyph)*MAP_TILES);
	v_push (all_dlevels, &new_level);
}

void dlv_set (int level)
{
	cur_level = level;
	cur_dlevel = dlv_lvl (level);
}

struct DLevel *dlv_lvl (int level)
{
	int i;
	struct DLevel *lvl;
	if (level == 0)
		return NULL;
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

Vector *dlv_things (int level)
{
	return dlv_lvl (level)->things;
}

Vector dlv_mons (int level)
{
	return dlv_lvl (level)->mons;
}

uint8_t *dlv_attr (int level)
{
	return dlv_lvl (level)->attr;
}

int dlv_up (int level)
{
	return dlv_lvl (level)->uplevel;
}

int dlv_dn (int level)
{
	return dlv_lvl (level)->dnlevel;
}

