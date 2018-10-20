/* sim.c */

#ifdef SIM

#include "include/sim.h"
#include "include/monst.h"
#include "include/generate.h"
#include "include/thing.h"

static int n = 0;
void sim_keypress (char in)
{
	if (in == 'f')
	{
		struct Monster p;
		init_mons (&p, MTYP_human);
		p.ctr.mode = CTR_AI_SIM_FARMER;
		p.level = 1; //mons[p.type].exp? TODO
		new_mons (cur_dlevel, 50+n/20, 150+n%20, &p);
		//struct Item myaxe = new_item (ITYP_FIRE_AXE);
		//item_put (&myaxe, (union ItemLoc) { .inv = {LOC_INV, th->ID, 0}});
		++ n;
	}
}

#endif /* SIM */

