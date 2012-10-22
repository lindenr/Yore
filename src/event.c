/* event.c */

#include "include/event.h"

void event_mhit (struct Monster *from, struct Monster *to, uint32_t atyp)
{
	switch (atyp)
	{
		case ATTK_HIT:
		{
			struct Item **it = mons_get_weap(from);
			if (!it || !(*it))
			{
				if (IS_PLAYER(from))
					pline("You hit the %s!", mons[to->type].name);
				else if (IS_PLAYER(to))
					pline("The %s hits you!", mons[from->type].name);
				else
					pline("The %s hits the %s!", mons[from->type].name,
						  mons[to->type].name);
			}
			else
			{
				if (IS_PLAYER(from))
					pline("You smite the %s!", mons[to->type].name);
				else if (IS_PLAYER(to))
					pline("The %s hits you!", mons[from->type].name);
				else
					pline("The %s hits the %s!", mons[from->type].name,
						  mons[to->type].name);
			}
			break;
		}
		case ATTK_TOUCH:
		{
			if (IS_PLAYER(from))
				pline("You touch the %s!", mons[to->type].name);
			else if (IS_PLAYER(to))
				pline("The %s touches you!", mons[from->type].name);
			break;
		}
		case ATTK_MAGIC:
		{
			pline("Magic attack not implemented");
			break;
		}
		case ATTK_CLAW:
		{
			if (IS_PLAYER(from))
				pline("You scratch the %s!", mons[to->type].name);
			else if (IS_PLAYER(to))
				pline("The %s scratches you!", mons[from->type].name);
			else
				pline("The %s scratches the %s!", mons[from->type].name,
					  mons[to->type].name);
			break;
		}
		case ATTK_BITE:
		{
			if (IS_PLAYER(from))
				pline("You bite the %s!", mons[to->type].name);
			else if (IS_PLAYER(to))
				pline("The %s bites you!", mons[from->type].name);
			else
				pline("The %s bites the %s!", mons[from->type].name,
					  mons[to->type].name);
			break;
		}
	}
}

void event_mkill (struct Monster *from, struct Monster *to)
{
	if (IS_PLAYER(from))
		pline("You kill the %s!", mons[to->type].name);
	else
		pline("The %s kills the %s!", mons[from->type].name, mons[to->type].name);
}

void event_mlevel (struct Monster *mons)
{
}

void event_anoise ()
{
}
