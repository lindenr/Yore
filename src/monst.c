/* monst.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/pline.h"
#include "include/rand.h"
#include "include/loop.h"
#include "include/save.h"
#include "include/vision.h"
#include "include/generate.h"
#include "include/rank.h"
#include "include/grammar.h"
#include "include/pline.h"
#include "include/all_mon.h"
#include "include/magic.h"
#include "include/output.h"
#include "include/event.h"
#include "include/graphics.h"
#include "include/dlevel.h"
#include "include/monst.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#define CONTROL_(c) ((c)-0x40)

struct player_status U;
char *s_hun[] = {
	"Full",
	"",							/* So that it takes up no space if there is
								   nothing to say */
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

	for (i = 0; i < BELT_JEWELS; ++i)
		U.jewel[i] = NUM_JEWELS;
	
	for (NUM_ITEMS = 0; items[NUM_ITEMS].name[0]; ++ NUM_ITEMS);

	for (i = 0; mons[i].name; ++i)
		if (!strcmp(mons[i].name, "human"))
			break;
	if (!mons[i].name)
		return;
	MTYP_HUMAN = i;

	for (i = 0; mons[i].name; ++i)
		if (!strcmp(mons[i].name, "Satan"))
			break;
	if (!mons[i].name)
		return;
	MTYP_SATAN = i;

	U.playing = PLAYER_STARTING;
}

void get_cinfo ()
{
	char in;

	gr_mvprintc (0, 0, "What role would you like to take up?");
	gr_mvprintc (0, glnumx - 11, "(q to quit)");
	gr_mvprintc (2, 3, "a     Assassin");
	gr_mvprintc (3, 3, "d     Doctor");
	gr_mvprintc (4, 3, "s     Soldier");
	gr_move (0, 37);
	gr_refresh ();

	do
	{
		in = gr_getch ();
		if (in == 'q' || in == 0x1B)
			return;
	}
	while (in != 'd' && in != 's' && in != 'a');

	gr_mvprintc (0, glnumx - 11, "           ");

	if (in == 's')
		U.role = 1;
	else if (in == 'd')
		U.role = 2;
	else if (in == 'a')
		U.role = 3;
	else
		panic ("get_cinfo failed");

	U.playing = PLAYER_PLAYING;
}

int expcmp (int p_exp, int m_exp)
{
	if (p_exp >= m_exp * 2)
		return 5;
	if ((p_exp * 20) + 10 >= m_exp * 19)
		return 50;
	if ((p_exp * 2) >= m_exp)
		return 1;
	return 0;
}

bool nogen (int mons_id)
{
	if (mons_id == MTYP_SATAN)
		return ((U.m_glflags&MGL_GSAT) != 0);

	return 0;
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
		if (array[i] > RN(total_weight))
			break;
		total_weight -= array[i];
	}
	if (i < NUM_MONS) return i;

	/* If the player has no exp this will happen */
	return 0;
}

inline int mons_get_wt (int type)
{
	return CORPSE_WEIGHTS[mons[type].flags >> 29];
}

ityp find_corpse (struct Thing *th)
{
	int type = th->thing.mons.type;
	ityp new_item;

	/* fill in the data */
	sprintf (new_item.name, "%s corpse", mons[type].name);
	new_item.ch   = ITEM_FOOD;
	new_item.type = IT_CORPSE;
	new_item.wt   = mons_get_wt(type);
	new_item.attr = type << 8;
	new_item.col  = mons[type].col;
	
	return new_item;
}

void mons_attack (struct Thing *th, int y, int x) /* each either -1, 0 or 1 */
{
	do_attack (th, get_sqmons(th->yloc + y, th->xloc + x, th->dlevel));
}

int mons_move (struct Thing *th, int y, int x) /* each either -1, 0 or 1 */
{
	if (th != player)
		if (!(x | y))
			return false;
	int can = can_amove (get_sqattr (th->yloc + y, th->xloc + x, th->thing.mons.level));
	/* like a an unmoveable boulder or something */
	if (!can)
		return false;
	/* you can and everything's fine, nothing doing */
	else if (can == 1)
	{
		thing_move (th, th->yloc+y, th->xloc+x);
		return true;
	}
	/* melee attack! */
	else if (can == 2)
	{
		mons_attack (th, y, x);
		return true;
	}
	/* off map or something */
	else if (can == -1)
	{
		/* nothing to do except return false (move not allowed) */
		return false;
	}
	/* shouldn't get to here -- been a mistake */
	panic ("mons_move() end reached");
	return false;
}

/* if a is in the range 0 <= a < 0x20 (' ' in ASCII) then a+64 is returned (so 
 * a backspace becomes '?'). This is the standard way to print non-printable
 * characters. */
inline char escape (unsigned char a)
{
	if (a < 0x20)
		return a + 0x40;
	else
		return a;
}

inline bool player_take_input (char in)
{
	int xmove = 0, ymove = 0;
	if (in == 'h')
		xmove = -1;
	else if (in == 'j')
		ymove = 1;
	else if (in == 'k')
		ymove = -1;
	else if (in == 'l')
		xmove = 1;
	else if (in == 'y')
	{
		ymove = -1;
		xmove = -1;
	}
	else if (in == 'u')
	{
		ymove = -1;
		xmove = 1;
	}
	else if (in == 'n')
	{
		ymove = 1;
		xmove = 1;
	}
	else if (in == 'b')
	{
		ymove = 1;
		xmove = -1;
	}
	else
		return (-1);

	return (mons_move (player, ymove, xmove));
}

void thing_move_level (struct Thing *th, int32_t where)
{
	uint32_t wh;
	if (where == 0) /* Uncontrolled teleport within level */
	{
		do
			wh = RN(MAP_TILES);
		while (!is_safe_gen (cur_dlevel, wh / MAP_WIDTH, wh % MAP_WIDTH));
		th->yloc = wh / MAP_WIDTH;
		th->xloc = wh % MAP_WIDTH;
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

struct Item *player_use_pack (char *msg, uint32_t accepted)
{
	struct Item *It = NULL;
	char in, cs[100];
	bool tried = false;

	do
	{
		if (tried)
			pline ("No such item.");
		tried = false;

		line_reset ();
		pack_get_letters (pmons.pack, cs);
		in = pask (cs, msg);
		if (in == '?')
		{
			show_contents (pmons.pack, accepted);
			gr_getch ();
			continue;
		}
		if (in == ' ' || in == 0x1B)
			break;
		if (in == '*')
		{
			show_contents (pmons.pack, ITCAT_ALL);
			gr_getch ();
			continue;
		}

		It = get_Itemc (pmons.pack, in);
		tried = true;
	}
	while (It == NULL);

	return It;
}

int mons_take_move (struct Thing *th)
{
	char in;
	struct Monster *self = &th->thing.mons;
	if (self->HP < self->HP_max && RN(50) < U.attr[AB_CO])
		self->HP += (self->level + 10) / 10;
	if (mons_eating(th))
		return true;
	if (th != player)
	{
		AI_Attack (th, player->yloc, player->xloc);
		return true;
	}
	while (1)
	{
		gr_refresh ();
		gr_move (th->yloc + 1, th->xloc);
		in = gr_getch();
		if (pline_check())
			line_reset();
		if (in == 'Q')
		{
			//if (!quit())
			{
				U.playing = PLAYER_LOSTGAME;
				return false;
			}
			continue;
		}
		if (in == 'S')
		{
			//if (!save(get_filename()))
			{
				U.playing = PLAYER_SAVEGAME;
				return false;
			}
			continue;
		}

		bool mv = player_take_input (in);
		if (mv != -1)
		{
			if (U.playing == PLAYER_WONGAME)
				return 0;
			if (mv)
				break;
		}
		else if (in == GRK_UP)
			gr_movecam (cam_yloc - 10, cam_xloc);
		else if (in == GRK_DN)
			gr_movecam (cam_yloc + 10, cam_xloc);
		else if (in == GRK_LF)
			gr_movecam (cam_yloc, cam_xloc - 10);
		else if (in == GRK_RT)
			gr_movecam (cam_yloc, cam_xloc + 10);
		else if (in == '.')
			break;
		else if (in == CONTROL_('Q') && U.magic == true)
		{
			pline("Press <m> to re-enter magic mode.");
			U.magic = false;
		}
		else if (U.magic)
		{
			if (player_magic(in))
				break;
		}
		else if (in == ',')
		{
			Vector ground = v_init (sizeof (int), 20);
			int n = to_buffer (th->yloc, th->xloc);

			LOOP_THING(n, i)
			{
				if (THING(n, i)->type == THING_ITEM)
					v_push (ground, &i);
			}

			if (ground->len == 1)
			{
				/* One item on ground -- pick up immediately. */
				if (pack_add (&pmons.pack, &THING(n, *(int*)v_at (ground, 0))->thing.item));
					rem_ref (n, i);
				v_free (ground);
			}
			else
			{
				/* Multiple items - ask which to pick up. */
				Vector pickup = v_init (sizeof(int), 20);

				/* Do the asking */
				mask_vec (n, pickup, ground);
				v_free (ground);

				/* Put items in ret_list into inventory. The loop
				 * continues until ret_list is done or the pack is full. */
				for (i = 0; i < pickup->len; ++ i)
				{
					/* Pick up the item; quit if the bag is full */
					if (!pack_add (&self->pack, &THING(n, *(int*)v_at (pickup, i))->thing.item))
						break;
					/* Remove item from main play */
					rem_ref (n, i);
				}
				v_free (pickup);
			}
		}
		else if (in == CONTROL_('P'))
		{
			pline_get_his ();
		}
		else if (in == 'm')
		{
			pline ("Press Ctrl+Q to leave magic mode.");
			U.magic = true;
		}
		else if (in == 'e')
		{
			struct Item *It = player_use_pack ("Eat what?", ITCAT_FOOD);
			if (It == NULL)
				continue;
			mons_eat (th, It);
		}
		else if (in == 'd')
		{
			struct Item *It = player_use_pack ("Drop what?", ITCAT_ALL);
			if (It == NULL)
				continue;
			unsigned u = PACK_AT(get_Itref(self->pack, It));
			self->pack.items[u] = NULL;
			new_thing (THING_ITEM, cur_dlevel, th->yloc, th->xloc, It);
		}
		else if (in == '>')
			thing_move_level(th, 0);
		else if (in == '<')
			thing_move_level(th, -1);
		else if (in == 'i')
		{
			show_contents (self->pack, ITCAT_ALL);
			continue;
		}
		else if (in == ':')
		{
			int k = 0;
			int n = to_buffer (th->yloc, th->xloc);
			LOOP_THING(n, i)
			{
				struct Thing *t_ = THING(n, i);
				if (t_->type != THING_ITEM)
					continue;
				char *line = get_inv_line (&THING(n, i)->thing.item);
				pline ("You%s see here %s. ", ((k++) ? " also" : ""), line);
				free (line);
			}
			if (k == 0)
				pline("You see nothing here. ");
		}
		else if (in == 'w')
		{
			struct Item *It = player_use_pack ("Wield what?", ITCAT_ALL);
			if (It == NULL)
				continue;
			unsigned u = PACK_AT(get_Itref(self->pack, It));
			self->pack.items[u] = NULL;
			new_thing (THING_ITEM, cur_dlevel, th->yloc, th->xloc, It);
			if (mons_unwield (th))
				mons_wield (th, It);
		}
		else
		{
			pline("Unknown command '%s%c'. ",
			      (escape(in) == in ? "" : "^"), escape(in));
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

	event_mkill (from, to);
	if (from == player)
	{
		if (to->thing.mons.type == MTYP_SATAN)
			U.playing = PLAYER_WONGAME;
		from->thing.mons.exp += mons[to->type].exp;
		update_level (from);
	}
	ityp c = find_corpse (to);
	struct Item it;
	it.type = c;
	it.attr = 0;
	it.name = NULL;
	it.cur_weight = 0;
	new_thing (THING_ITEM, cur_dlevel, to->yloc, to->xloc, &it);
	thing_free (to);
}

/* TODO is it polymorphed? */
inline bool mons_edible (struct Thing *th, struct Item *item)
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
			U.hunger -= (item->cur_weight) >> 4;	/* U.hunger is signed */
			line_reset ();
			pline ("You finish eating.");
		}
		th->thing.mons.status &= ~M_EATING;
		th->thing.mons.eating = NULL;
		th->thing.mons.pack.items[PACK_AT(get_Itref(th->thing.mons.pack, item))] = NULL;
		return false;
	}
	hunger_loss = RN(25) + 50;
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
			pline("You can't eat that!");
		return;
	}

	if ((th->thing.mons.status) & M_EATING)
	{
		if (th == player)
			pline("You're already eating!");
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
			line_reset();
			pline ("You can't. It's cursed.");
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
		line_reset ();
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
			line_reset ();
			pline ("You can't wear that!");
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
	char *posv;
	for (t = 0; t < A_NUM; ++t)
		if ((mons[type].attacks[t][2] & 0xFFFF) == ATTK_PASS)
			break;
	if (t >= A_NUM)
		return;

	switch (mons[type].attacks[t][2] >> 16)
	{
		case ATYP_ACID:
		{
			posv = malloc(strlen(mons[type].name) + 5);
			gram_pos (posv, (char *)mons[type].name);
			if (from == player)
				pline ("You splash the %s with acid!", mons[type].name);
			else if (to == player)
				pline ("You are splashed by the %s acid!", posv);
		}
	}
}

int mons_get_st (struct Thing *th)
{
	if (th == player)
		return U.attr[AB_ST];
	return 1;
}

inline void do_attack (struct Thing *from, struct Thing *to)
{
	int t, strength, type = from->thing.mons.type;
	int *toHP = &to->thing.mons.HP;

	for (t = 0; t < A_NUM; ++t)
	{
		if (!mons[type].attacks[t][0])
			break;

		switch (mons[type].attacks[t][2] & 0xFFFF)
		{
			case ATTK_HIT:
			{
				struct Item **it = mons_get_weap(from);
				if (!it || !(*it))
				{
					strength = RN(mons_get_st(from)) >> 1;
					*toHP -=
						RND(mons[type].attacks[t][0],
							mons[type].attacks[t][1]) +
                        strength +
					    RN(3 * from->thing.mons.level);
				}
				else
				{
					int attr = (*it)->type.attr;
					strength = RN(mons_get_st(from)) >> 1;
					*toHP -= RND(attr & 15, (attr >> 4) & 15) + strength;
				}

				mons_passive_attack (to, from);
				break;
			}
			case ATTK_TOUCH:
			{
				*toHP -=
					RND(mons[type].attacks[t][0],
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
					RND(mons[type].attacks[t][0],
						mons[type].attacks[t][1]);

				mons_passive_attack (to, from);
				break;
			}
			case ATTK_BITE:
			{
				*toHP -=
					RND(mons[type].attacks[t][0],
						mons[type].attacks[t][1]);

				mons_passive_attack (to, from);
				break;
			}
		}
		event_mhit (from, to, mons[type].attacks[t][2] & 0xFFFF);

		if ((*toHP) <= 0)
		{
			mons_dead (from, to);
			break;
		}
	}
}

void player_dead (const char *msg, ...)
{
	va_list args;
	char *actual = malloc(sizeof(char) * 80);

	va_start(args, msg);
	if (msg[0] == '\0')
		msg = "You die...";
	vsprintf(actual, msg, args);
	line_reset();
	pline(actual);
	free(actual);
	gr_getch();
	va_end(args);

	U.playing = PLAYER_LOSTGAME;
}

bool player_magic (char c)
{
	if (c == 'j')
	{
		//
	}
	if (!magic_plspell(c))
	{
		pline("Unknown spell '%s%c'. ",
			  (escape(c) == c ? "" : "^"), escape(c));
		return false;
	}
	return true;
}

/* Rudimentary AI system -- move towards player if player is visible. */
int AI_Attack (struct Thing *th, int toy, int tox)
{
	int xmove = 0, ymove = 0;
	bres_start (th->yloc, th->xloc, NULL, sq_attr);
	if (!bres_draw (toy, tox))
	{
		mons_move (th, RN(3) - 1, RN(3) - 1);
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
	if (!mons_move (th, ymove, xmove))
		if (!mons_move (th, ymove, 0))
			if (!mons_move (th, 0, xmove))
				if (!mons_move (th, -ymove, xmove))
					mons_move (th, RN(3) - 1, RN(3) - 1);
	return 1;
}
