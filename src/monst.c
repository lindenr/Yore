/* monst.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/panel.h"
#include "include/rand.h"
#include "include/loop.h"
#include "include/save.h"
#include "include/vision.h"
#include "include/generate.h"
#include "include/words.h"
#include "include/event.h"
#include "include/graphics.h"
#include "include/dlevel.h"
#include "include/monst.h"
#include "include/player.h"
#include "include/timer.h"
#include "include/skills.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>


struct player_status U;
char *s_hun[] = {
	"Full",
	"",
	"Hungry",
	"Hungry!",
	"Starved",
	"Dead"
};

char *get_hungerstr()
{
	if (U.hunger < HN_LIMIT_1)
		return s_hun[0];
	if (U.hunger < HN_LIMIT_2)
		return s_hun[1];
	if (U.hunger < HN_LIMIT_3)
		return s_hun[2];
	if (U.hunger < HN_LIMIT_4)
		return s_hun[3];
	if (U.hunger < HN_LIMIT_5)
		return s_hun[4];
	return s_hun[5];
}

bool digesting()
{
	return true;
}

void setup_U ()
{
	int i;

	U.playing = PLAYER_ERROR;	/* If this function returns early */
	U.hunger = 100;
	U.luck = 0;

	for (i = 0; i < 6; ++i)
		U.attr[i] = 10;

	for (NUM_ITEMS = 0; items[NUM_ITEMS].name[0]; ++ NUM_ITEMS);

	U.playing = PLAYER_STARTING;
}

void get_cinfo ()
{
	U.role = 1;
	U.playing = PLAYER_PLAYING;
}

int expcmp (int p_exp, int m_exp)
{
	if (p_exp >= m_exp * 2)
		return 5;
	if ((p_exp * 20) + 20 >= m_exp * 19)
		return 50;
	if ((p_exp * 2) >= m_exp)
		return 1;
	return 0;
}

bool nogen (int mons_id)
{
	if (mons_id == MTYP_SATAN)
		return ((U.m_glflags&MGL_GSAT) != 0);

	return false;
}

int player_gen_type ()
{
	int i, array[NUM_MONS];
	uint32_t p_exp = pmons.exp;
	uint32_t total_weight = 0;

	for (i = 0; i < NUM_MONS; ++i)
	{
		if (nogen(i)) continue; /* genocided or unique and generated etc */
		array[i] = expcmp(p_exp, mons[i].exp);
		total_weight += array[i];
	}

	for (i = 0; i < NUM_MONS; ++i)
	{
		if (array[i] > rn(total_weight))
			break;
		total_weight -= array[i];
	}
	if (i < NUM_MONS) return i;

	/* If the player has no exp this will happen */
	return 0;
}

int mons_get_wt (int type)
{
	return CORPSE_WEIGHTS[mons[type].flags >> 29];
}

void make_corpse (ityp *typ, struct Thing *th)
{
	int type = th->thing.mons.type;

	/* fill in the data */
	snprintf (typ->name, ITEM_NAME_LENGTH, "%s corpse", mons[type].name);
	typ->ch   = ITEM_FOOD;
	typ->type = IT_CORPSE;
	typ->wt   = mons_get_wt(type);
	typ->attr = type << 8;
	typ->col  = mons[type].col;
}

void mons_attack (struct Thing *th, int y, int x) /* each either -1, 0 or 1 */
{
	do_attack (th, get_sqmons(dlv_things(th->dlevel), th->yloc + y, th->xloc + x));
}

/* Return values:
 * 0 = failed to move; 1 = moved as desired;
 * 2 = did not move as desired but used turn */
int mons_move (struct Thing *th, int y, int x, int final) /* each either -1, 0 or 1 */
{
	if (th != player)
		if (!(x | y))
			return 0;
	int can = can_amove (get_sqattr (dlv_things(th->dlevel), th->yloc + y, th->xloc + x));
	/* like a an unmoveable boulder or something */
	if (!can)
		return 0;
	/* you can and everything's fine, nothing doing */
	else if (can == 1)
	{
		if (final)
			mons_usedturn (th);
		thing_move (th, th->dlevel, th->yloc+y, th->xloc+x);
		return 1;
	}
	/* melee attack! */
	else if (can == 2)
	{
		if (final)
			mons_usedturn (th);
		mons_attack (th, y, x);
		return 2;
	}
	/* off map or something */
	else if (can == -1)
	{
		/* nothing to do except return 0 (move not allowed) */
		return 0;
	}
	/* shouldn't get to here -- been a mistake */
	panic ("mons_move() end reached");
	return 0;
}

void mons_usedturn (struct Thing *th)
{
	if (th != player)
		return;
	th->thing.mons.status &= ~M_CHARGE;
	int i;
	for (i = 0; i < cur_dlevel->mons->len; ++ i)
	{
		int *id = v_at (cur_dlevel->mons, i);
		(*(struct Thing **) v_at(all_ids, *id))->thing.mons.boxflags = 0;
	}
}

int player_take_input (char in)
{
	int xmove, ymove;
	pl_move (&ymove, &xmove, (uint32_t) in);
	if (xmove == 0 && ymove == 0)
		return (-1);

	return (mons_move (player, ymove, xmove, 1));
}

void thing_move_level (struct Thing *th, int32_t where)
{
	uint32_t wh;
	if (where == 0) /* Uncontrolled teleport within level */
	{
		do
			wh = rn(map_graph->a);
		while (!is_safe_gen (cur_dlevel, wh / map_graph->w, wh % map_graph->w));
		th->yloc = wh / map_graph->w;
		th->xloc = wh % map_graph->w;
	}
	else if (where == 1) /* go up stairs */
	{
	}
	else if (where == -1) /* go down stairs */
	{
	}
	else
	{
		where >>= 1;
	}
}

char escape (unsigned char a)
{
	if (a < 0x20)
		return a + 0x40;
	else
		return a;
}

int mons_take_move (struct Thing *th)
{
	char in;
	struct Monster *self = &th->thing.mons;
	if (self->HP < self->HP_max && rn(50) < U.attr[AB_CO])
		self->HP += (self->level + 10) / 10;
	self->HP_rec = ((10.0 + self->level)/10) * ((float)U.attr[AB_CO] / 50.0);
	if (mons_eating(th))
		return true;
	if (th != player)
	{
		AI_Attack (th, player->yloc, player->xloc);
		return true;
	}
	while (1)
	{
		draw_map ();
		p_pane ();

		gra_cmove (map_graph, th->yloc, th->xloc);

		uint32_t key = gr_getfullch();

		in = (char) key;

		int mv = player_take_input (in);
		if (mv != -1)
		{
			if (gra_nearedge (map_graph, player->yloc, player->xloc))
				gra_centcam (map_graph, player->yloc, player->xloc);
			if (U.playing == PLAYER_WONGAME)
				return false;
			if (mv)
				break;
		}
		else
		{
			int res = key_lookup (key);
			if (res == 1)
				break;
			else if (res == 0)
				continue;
			else
				return false;
		}
	}
	return true;
}

void mons_dead (struct Thing *from, struct Thing *to)
{
	if (to == player)
	{
		player_dead("");
		return;
	}

	if (!from)
		goto skip1;
	event_mkill (from, to);
	if (from == player)
	{
		if (to->thing.mons.type == MTYP_SATAN)
			U.playing = PLAYER_WONGAME;
		pmons.exp += mons[to->thing.mons.type].exp;
		//update_level (from);
	}
skip1:;
	/* add corpse */
	struct Item corpse;
	make_corpse (&corpse.type, to);
	corpse.attr = 0;
	corpse.name = NULL;
	corpse.cur_weight = 0;
	new_thing (THING_ITEM, dlv_lvl (to->dlevel), to->yloc, to->xloc, &corpse);

	/* remove dead monster */
	rem_id (to->ID);
}

int mons_prhit (struct Thing *from, struct Thing *to, int energy)
{
	to->thing.mons.HP -= energy/2;
	if (to->thing.mons.HP <= 0)
		mons_dead (from, to);
	return 1;
}

/* TODO is it polymorphed? */
bool mons_edible (struct Thing *th, struct Item *item)
{
	return (item->type.ch == ITEM_FOOD);
}

bool mons_eating (struct Thing *th)
{
	int hunger_loss;
	struct Item *item = th->thing.mons.eating;
	if (!item)
		return false;
	if (item->cur_weight <= 1200)
	{
		if (th == player)
		{
			U.hunger -= (item->cur_weight) >> 4;
			p_msg ("You finish eating.");
		}
		th->thing.mons.status &= ~M_EATING;
		th->thing.mons.eating = NULL;
		th->thing.mons.pack.items[PACK_AT(get_Itref(th->thing.mons.pack, item))] = NULL;
		return false;
	}
	hunger_loss = rn(25) + 50;
	item->cur_weight -= hunger_loss << 4;
	if (th == player)
		U.hunger -= hunger_loss;
	return true;
}

void mons_eat (struct Thing *th, struct Item *item)
{
	if (!mons_edible (th, item))
	{
		if (th == player)
			p_msg("You can't eat that!");
		return;
	}

	if ((th->thing.mons.status) & M_EATING)
	{
		if (th == player)
			p_msg("You're already eating!");
		return;
	}
	th->thing.mons.status |= M_EATING;
	th->thing.mons.eating = item;
	if (!item->cur_weight)
		item->cur_weight = item->type.wt;
}

inline void *mons_get_weap (struct Thing *th)
{
	return &th->thing.mons.wearing.rweap;
}

bool mons_unwield (struct Thing *th)
{
	struct Item **pweap = mons_get_weap (th);
	struct Item *weap = *pweap;
	if (weap == NULL)
		return true;
	if (weap->attr & ITEM_CURS)
	{
		if (th == player)
		{
			p_msg ("You can't. It's cursed.");
		}
		return false;
	}
	weap->attr ^= ITEM_WIELDED;
	*pweap = NULL;
	return true;
}

bool mons_wield (struct Thing *th, struct Item *it)
{
	th->thing.mons.wearing.rweap = it;
	it->attr ^= ITEM_WIELDED;
	if (th == player)
	{
		item_look (it);
	}
	return true;
}

bool mons_wear (struct Thing *th, struct Item *it)
{
	if (it->type.ch != ITEM_ARMOUR)
	{
		if (th == player)
		{
			p_msg ("You can't wear that!");
		}
		return false;
	}

	switch (it->type.type)
	{
		case IT_GLOVES:
		{
			th->thing.mons.wearing.hands = it;
			break;
		}
		default:
		{
			panic ("Armour not recognised");
		}
	}
	return true;
}

void mons_passive_attack (struct Thing *from, struct Thing *to)
{
	uint32_t t;
	int type = from->thing.mons.type;
	char posv[30];
	for (t = 0; t < A_NUM; ++t)
		if ((mons[type].attacks[t][2] & 0xFFFF) == ATTK_PASS)
			break;
	if (t >= A_NUM)
		return;

	switch (mons[type].attacks[t][2] >> 16)
	{
		case ATYP_ACID:
		{
			w_pos (posv, (char *)mons[type].name, 30);
			if (from == player)
				p_msg ("You splash the %s with acid!", mons[type].name);
			else if (to == player)
				p_msg ("You are splashed by the %s acid!", posv);
		}
	}
}

int mons_get_st (struct Thing *th)
{
	if (th == player)
		return U.attr[AB_ST];
	return 1;
}

void mons_blast (struct Thing *from, struct Thing *to, int siz)
{
	to->thing.mons.HP -= siz*(siz+2);
	if (to->thing.mons.HP <= 0)
		mons_dead (from, to);
}

void mons_box (struct Thing *mons, BoxType type)
{
	mons->thing.mons.boxflags |= 1<<type;
}

int mons_charge_bonus (struct Thing *from)
{
	return 5*sk_lvl (from, SK_CHARGE);
}

void do_attack (struct Thing *from, struct Thing *to)
{
	int t, strength, type = from->thing.mons.type;
	int *toHP = &to->thing.mons.HP;
	int bonus = 0;
	if (from->thing.mons.status & M_CHARGE)
		bonus += mons_charge_bonus (from);

	for (t = 0; t < A_NUM; ++t)
	{
		if (!mons[type].attacks[t][0])
			break;

		switch (mons[type].attacks[t][2] & 0xFFFF)
		{
			case ATTK_HIT:
			{
				struct Item **it = mons_get_weap(from);
				int dmg;
				if (!it || !(*it))
				{
					strength = rn(mons_get_st(from)) >> 1;
					dmg =
						rnd(mons[type].attacks[t][0],
							mons[type].attacks[t][1]) +
                        strength +
					    rn(1 + 3*from->thing.mons.level);
				}
				else
				{
					int attr = (*it)->type.attr;
					strength = rn(mons_get_st(from)) >> 1;
					dmg = rnd(attr & 15, (attr >> 4) & 15) + strength;
					//printf("%d %d\n", *toHP, *toHP - damage);
				}

				*toHP -= damage + bonus;
				mons_passive_attack (to, from);
				break;
			}
			case ATTK_TOUCH:
			{
				*toHP -=
					rnd(mons[type].attacks[t][0],
						mons[type].attacks[t][1]);

				mons_passive_attack (to, from);
				break;
			}
			case ATTK_MAGIC:
			{
				break;
			}
			case ATTK_CLAW:
			{
				*toHP -=
					rnd(mons[type].attacks[t][0],
						mons[type].attacks[t][1]);

				mons_passive_attack (to, from);
				break;
			}
			case ATTK_BITE:
			{
				*toHP -=
					rnd(mons[type].attacks[t][0],
						mons[type].attacks[t][1]);

				mons_passive_attack (to, from);
				break;
			}
		}
		event_mhit (from, to, mons[type].attacks[t][2] & 0xFFFF);

		if ((*toHP) <= 0)
		{
			mons_dead (from, to);
			return;
		}
	}
}

struct Item *player_use_pack (char *msg, uint32_t accepted)
{
	struct Item *It = NULL;
	char in = show_contents (pmons.pack, accepted, msg);
	//char in, cs[100];
	//bool tried = false;
/*
	do
	{
		if (tried)
			p_msg ("No such item.");
		tried = false;

		pack_get_letters (pmons.pack, cs);
		in = p_ask (cs, msg);
		if (in == '?')
		{
			gr_getch ();
			continue;
		}
		if (in == ' ' || in == 0x1B)
			break;
		if (in == '*')
		{
			show_contents (pmons.pack, ITCAT_ALL, "Inventory");
			gr_getch ();
			continue;
		}

		It = get_Itemc (pmons.pack, in);
		tried = true;
	}
	while (It == NULL);*/
	It = get_Itemc (pmons.pack, in);

	return It;
}

int player_sense (int yloc, int xloc, int senses)
{
	if (senses&SENSE_VISION)
	{
		if (bres_draw (player->yloc, player->xloc, NULL, dlv_attr(player->dlevel), NULL, yloc, xloc))
			return 1;
	}
	if (senses&SENSE_HEARING)
	{
		return 1;
	}
	return 0;
}

void player_dead (const char *msg, ...)
{
	va_list args;
	char *actual = malloc(80);

	va_start (args, msg);
	if (msg[0] == '\0')
		msg = "You die...";
	vsnprintf (actual, 80, msg, args);
	p_msg (actual);
	free (actual);
	gr_getch ();
	va_end (args);

	U.playing = PLAYER_LOSTGAME;
}

/* Rudimentary AI system -- move towards player if player is visible. */
int AI_Attack (struct Thing *th, int toy, int tox)
{
	int xmove = 0, ymove = 0;
	if (!bres_draw (player->yloc, player->xloc, NULL, dlv_attr(player->dlevel), NULL, toy, tox))
	{
		mons_move (th, rn(3) - 1, rn(3) - 1, 1);
		return 1;
	}

	if      (th->yloc < toy)
		ymove = 1;
	else if (th->yloc > toy)
		ymove = -1;
	if      (th->xloc < tox)
		xmove = 1;
	else if (th->xloc > tox)
		xmove = -1;
	if (!mons_move (th, ymove, xmove, 1))
		if (!mons_move (th, ymove, 0, 1))
			if (!mons_move (th, 0, xmove, 1))
				if (!mons_move (th, -ymove, xmove, 1))
					return mons_move (th, rn(3) - 1, rn(3) - 1, 1);
	return 1;
}

