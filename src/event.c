/* event.c */

#include "include/event.h"
#include "include/pline.h"

void event_mhit (struct Thing *from, struct Thing *to, uint32_t atyp)
{
	int ftyp = from->thing.mons.type, ttyp = to->thing.mons.type;
	const char *fname = mons[ftyp].name, *tname = mons[ttyp].name;
	switch (atyp)
	{
		case ATTK_HIT:
		{
			struct Item **it = mons_get_weap(from);
			if (!it || !(*it))
			{
				if (from == player)
					pline("You hit the %s!", tname);
				else if (to == player)
					pline("The %s hits you!", fname);
				else
					pline("The %s hits the %s!", fname, tname);
			}
			else
			{
				if (from == player)
					pline("You smite the %s!", tname);
				else if (to == player)
					pline("The %s hits you!", fname);
				else
					pline("The %s hits the %s!", fname, tname);
			}
			break;
		}
		case ATTK_TOUCH:
		{
			if (from == player)
				pline("You touch the %s!", tname);
			else if (to == player)
				pline("The %s touches you!", fname);
			break;
		}
		case ATTK_MAGIC:
		{
			pline("Magic attack not implemented");
			break;
		}
		case ATTK_CLAW:
		{
			if (from == player)
				pline("You scratch the %s!", tname);
			else if (to == player)
				pline("The %s scratches you!", fname);
			else
				pline("The %s scratches the %s!", fname, tname);
			break;
		}
		case ATTK_BITE:
		{
			if (from == player)
				pline("You bite the %s!", tname);
			else if (to == player)
				pline("The %s bites you!", fname);
			else
				pline("The %s bites the %s!", fname, tname);
			break;
		}
	}
}

void event_mkill (struct Thing *from, struct Thing *to)
{
	int ftyp = from->thing.mons.type, ttyp = to->thing.mons.type;
	const char *fname = mons[ftyp].name, *tname = mons[ttyp].name;
	if (from == player)
		pline("You kill the %s!", tname);
	else
		pline("The %s kills the %s!", fname, tname);
}

void event_mlevel (struct Thing *th)
{
}

void event_anoise ()
{
}
