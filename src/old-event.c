/* event.c */

#include "include/old-event.h"
#include "include/panel.h"
#include "include/dlevel.h"

#include <stdarg.h>
#include <stdio.h>

void event_mhit (struct Thing *from, struct Thing *to, uint32_t atyp)
{
	mons_box (to, BOX_HIT);
	int know = player_sense (to->yloc, to->xloc, SENSE_VISION);
	if (!know)
		return;

	int ftyp = from->thing.mons.type, ttyp = to->thing.mons.type;
	const char *fname = all_mons[ftyp].name, *tname = all_mons[ttyp].name;
	switch (atyp)
	{
		case ATTK_HIT:
		{
			struct Item **it = mons_get_weap(from);
			if (!it || !(*it))
			{
				if (from == player)
					p_msg ("You hit the %s!", tname);
				else if (to == player)
					p_msg ("The %s hits you!", fname);
				else
					p_msg ("The %s hits the %s!", fname, tname);
			}
			else
			{
				if (from == player)
					p_msg ("You smite the %s!", tname);
				else if (to == player)
					p_msg ("The %s hits you!", fname);
				else
					p_msg ("The %s hits the %s!", fname, tname);
			}
			break;
		}
		case ATTK_TOUCH:
		{
			if (from == player)
				p_msg ("You touch the %s!", tname);
			else if (to == player)
				p_msg ("The %s touches you!", fname);
			break;
		}
		case ATTK_MAGIC:
		{
			p_msg ("Magic attack not implemented");
			break;
		}
		case ATTK_CLAW:
		{
			if (from == player)
				p_msg ("You scratch the %s!", tname);
			else if (to == player)
				p_msg ("The %s scratches you!", fname);
			else
				p_msg ("The %s scratches the %s!", fname, tname);
			break;
		}
		case ATTK_BITE:
		{
			if (from == player)
				p_msg ("You bite the %s!", tname);
			else if (to == player)
				p_msg ("The %s bites you!", fname);
			else
				p_msg ("The %s bites the %s!", fname, tname);
			break;
		}
	}
}

void event_mkill (struct Thing *from, struct Thing *to)
{
	mons_box (from, BOX_KILL);
	if (!player_sense (from->yloc, from->xloc, SENSE_VISION))
		return;

	int ftyp = from->thing.mons.type, ttyp = to->thing.mons.type;
	const char *fname = all_mons[ftyp].name, *tname = all_mons[ttyp].name;
	if (from == player)
		p_msg ("You kill the %s!", tname);
	else
		p_msg ("The %s kills the %s!", fname, tname);
}

void event_mlevel (struct Thing *th)
{
}

void event_anoise ()
{
}
