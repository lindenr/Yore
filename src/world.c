/* world.c */

#include "include/world.h"
#include "include/event.h"
#include "include/dlevel.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/vector.h"
#include "include/heap.h"
#include "include/panel.h"
#include "include/generate.h"
#include "include/thing.h"

struct World world;

void world_init (char *player_name)
{
	Tick tick_init = 0;
	EvID evID_init = 0;
	world = (struct World) {
		tick_init,
		player_name,
		evID_init,
		h_dinit (sizeof(struct QEv), qev_lt),
		v_dinit (sizeof(DLevel)),
		v_dinit (sizeof(Item)),
		v_dinit (sizeof(Mons)),
		NULL
	};

	struct Item_internal dummy_item = {0};
	v_push (world.items, &dummy_item);

	struct Monster_internal dummy_mons = {0};
	v_push (world.mons, &dummy_mons);

	int dlevel = dlv_init (-1, -1, 11, 50, 150);
	struct Item_internal myitem = new_item (ITYP_FIRE_TURRET);
	it_create (&myitem, (union ItemLoc) { .dlvl = {LOC_DLVL, dlevel, 0, 17, 75}});
	MonsID mons = gen_player (dlevel, 0, 24, 61, world.player_name);
	//dlv_fill_player_dist (dlevel);
	update_knowledge (mons);
}

