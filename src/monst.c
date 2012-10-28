/* monst.c */

#include "include/all.h"
#include "include/monst.h"
#include "include/pline.h"
#include "include/rand.h"
#include "include/util.h"
#include "include/bool.h"
#include "include/loop.h"
#include "include/save.h"
#include "include/vision.h"
#include "include/generate.h"
#include "include/rank.h"
#include "include/grammar.h"
#include "include/pline.h"
#include "include/util.h"
#include "include/all_mon.h"
#include "include/magic.h"
#include "include/output.h"
#include "include/event.h"
#include "include/graphics.h"

#include <stdarg.h>
#include <stdio.h>

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

void setup_U()
{
	int i;

	U.playing = PLAYER_ERROR;	/* If this function returns prematurely */
	U.hunger = 100;
	U.luck = 0;

	for (i = 0; i < 6; ++i)
		U.attr[i] = 10;

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

void get_cinfo()
{
	char in;

	gr_mvprintc(0, 0, "What role would you like to take up?");
	gr_mvprintc(0, glnumx - 11, "(q to quit)");
	gr_mvprintc(2, 3, "a     Assassin");
	gr_mvprintc(3, 3, "d     Doctor");
	gr_mvprintc(4, 3, "s     Soldier");
	gr_move(0, 37);
	gr_refresh();

	do
	{
		in = gr_getch();
		if (in == 'q' || in == 0x1B)
			return;
	}
	while (in != 'd' && in != 's' && in != 'a');

	gr_mvprintc(0, glnumx - 11, "           ");

	if (in == 's')
		U.role = 1;
	else if (in == 'd')
		U.role = 2;
	else if (in == 'a')
		U.role = 3;
	else
		return;					/* shouldn't get here -- we will quit */

	U.playing = PLAYER_PLAYING;
}

uint32_t expcmp(uint32_t p_exp, uint32_t m_exp)
{
	if (p_exp >= m_exp * 2)
		return 5;
	if ((p_exp * 20) + 10 >= m_exp * 19)
		return 50;
	if ((p_exp * 2) >= m_exp)
		return 1;
	return 0;
}

bool nogen(uint32_t mons_id)
{
	if (mons_id == MTYP_SATAN)
		return ((U.m_glflags&MGL_GSAT) != 0);

	return 0;
}

uint32_t player_gen_type()
{
	uint32_t i, array[NUM_MONS];
	uint32_t p_exp = get_pmonster()->exp;
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

inline int mons_get_wt(struct Monster *self)
{
	return CORPSE_WEIGHTS[mons[self->type].flags >> 29];
}

struct item_struct *find_corpse(struct Monster *m)
{
	/* mallocate for new type of corpse */
	struct item_struct *new_item = malloc(sizeof(struct item_struct));

	/* fill in the item_struct data */
	sprintf(new_item->name, "%s corpse", mons[m->type].name);
	new_item->ch = ITEM_FOOD;
	new_item->type = IT_CORPSE;
	new_item->wt = mons_get_wt(m);
	new_item->attr = m->type << 8;
	new_item->col = mons[m->type].col;
	
	return new_item;
}

void mons_attack(struct Monster *self, int y, int x)	/* each either -1, 0
														   or 1 */
{
	struct Thing *th = get_thing(self);
	apply_attack(self,
				 get_square_monst(th->yloc + y, th->xloc + x, self->level));
}

int mons_move(struct Monster *self, int y, int x)	/* each either -1, 0 or 1 */
{
	if (!IS_PLAYER(self))
		if (!(x | y))
			return false;
	struct Thing *t = get_thing(self);
	int can = can_move_to(get_square_attr(t->yloc + y, t->xloc + x, self->level));
	/* like a an unmoveable boulder or something */
	if (!can)
		return false;
	/* you can and everything's fine, nothing doing */
	else if (can == 1)
	{
		thing_move (t, t->yloc+y, t->xloc+x);
		return true;
	}
	/* melee attack! */
	else if (can == 2)
	{
		mons_attack(self, y, x);
		return true;
	}
	/* off map or something */
	else if (can == -1)
	{
		/* nothing to do except return false (move not allowed) */
		return false;
	}
	/* shouldn't get to here -- been a mistake */
	return false;
}

/* if a is in the range 0 <= a < 0x20 (' ' in ASCII) then a+64 is returned (so 
   a backspace becomes '?'). This is the standard way to print non-printable
   characters. */
inline char escape(unsigned char a)
{
	if (a < 0x20)
		return a + 0x40;
	else
		return a;
}

inline bool mons_take_input(struct Thing * th, char in)
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

	return (mons_move(th->thing, ymove, xmove));
}

void thing_move_level(struct Thing *th, int32_t where)
{
	uint32_t wh;
	if (where == 0)				/* Uncontrolled teleport within level */
	{
		do
			wh = RN(1680);
		while (!is_safe_gen(wh / 80, wh % 80));
		th->yloc = wh / 80;
		th->xloc = wh % 80;
	}
	else if (where == 1)		/* go up stairs */
	{
	}
	else if (where == -1)		/* go down stairs */
	{
	}
	else						/* levelport -- always uncontrolled, see
								   thing_move_level_c(TODO) for controlled
								   teleportation */
	{
		where >>= 1;			/* LSB is unused */
	}
}

struct Item *player_use_pack(struct Thing *player, char *msg, bool * psc,
							 uint32_t accepted)
{
	struct Item *It = NULL;
	char in, cs[100];
	struct Monster *self = player->thing;
	bool tried = false;

	do
	{
		if (tried)
			pline("No such item.");
		tried = false;

		line_reset();
		pack_get_letters(self->pack, cs);
		in = pask(cs, msg);
		if (in == '?')
		{
			show_contents(self->pack, accepted);
			gr_getch();
			unscreenshot();
			continue;
		}
		if (in == ' ' || in == 0x1B)
			break;
		if (in == '*')
		{
			show_contents(self->pack, ITCAT_ALL);	/* everything */
			gr_getch();
			unscreenshot();
			continue;
		}

		It = get_Itemc(self->pack, in);
		tried = true;
	}
	while (It == NULL);

	return It;
}

int mons_take_move (struct Monster *self)
{
	char in;
	if (self->HP < self->HP_max && RN(50) < U.attr[AB_CO])
		self->HP += (self->level + 10) / 10;
	if (mons_eating(self))
		return true;
	struct Thing *th = get_thing(self);
	bool screenshotted = false;
	if (!IS_PLAYER(self))
	{
		struct Thing *pl = get_player();
		AI_Attack(th->yloc, th->xloc, pl->yloc, pl->xloc, self);
		return true;
	}
	while (1)
	{
		if (mons_eating(self))
			return true;
		gr_refresh ();
		gr_move (th->yloc + 1, th->xloc);
		//if (screenshotted)
		//{
		//	screenshotted = false;
		//	unscreenshot();
		//}
		in = gr_getch();
		//if (pline_check())
		//	line_reset();
		if (in == 'Q')
		{
			if (!quit())
			{
				U.playing = PLAYER_LOSTGAME;
				return false;
			}
			continue;
		}
		if (in == 'S')
		{
			if (!save(get_filename()))
			{
				U.playing = PLAYER_SAVEGAME;
				return false;
			}
			continue;
		}

		bool mv = mons_take_input(th, in);
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
			screenshot();
			screenshotted = true;
			struct List Li = LIST_INIT;
			struct Thing *t_;
			ITER_THINGS(li, num)
			{
				t_ = li->data;
				if (t_->type != THING_ITEM)
					continue;
				if (t_->xloc == th->xloc && t_->yloc == th->yloc)
				{
					push_back(&Li, t_);
				}
			}
			if (Li.beg == Li.end)	/* One element in list - pick up
									   immediately. */
			{
				pack_add(&self->pack,
						 ((struct Thing *)(Li.beg->data))->thing);
				rem_by_data(((struct Thing *)(Li.beg->data))->thing);
				free(Li.beg);
			}
			else			/* Multiple items - ask which to pick up. */
			{
				screenshotted = true;
				struct List ret_list = LIST_INIT;

				/* Ask which */
				mask_list(&ret_list, Li);

				/* Empty Li */
				for (li = Li.beg; iter_good(li); next_iter(&li))
				{
					if (li != Li.beg)
						free(li->prev);
				}
				free(li->prev);

				/* Put items in ret_list into inventory. The loop
				   continues until ret_list is done or the pack is full. */
				for (li = ret_list.beg;
					 iter_good(li)
					 && pack_add(&self->pack,
								 ((struct Thing *)li->data)->thing);
					 next_iter(&li))
				{
					/* Remove selected items from main play */
					rem_by_data(((struct Thing *)li->data)->thing);
				}
			}
		}
		else if (in == CONTROL_('P'))
		{
			pline_get_his();
		}
		else if (in == 'm')
		{
			pline("Press Ctrl+Q to leave magic mode.");
			U.magic = true;
		}
		else if (in == 'e')
		{
			struct Item *It =
				player_use_pack(th, "Eat what?", &screenshotted,
								ITCAT_FOOD);
			if (It == NULL)
				continue;
			mons_eat(self, It);
		}
		else if (in == 'd')
		{
			struct Item *It =
				player_use_pack(th, "Drop what?", &screenshotted, -1);
			if (It == NULL)
				continue;
			unsigned u = PACK_AT(get_Itref(self->pack, It));
			self->pack.items[u] = NULL;
			new_thing(THING_ITEM, th->yloc, th->xloc, It);
		}
		else if (in == '>')
			thing_move_level(th, 0);
		else if (in == '<')
			thing_move_level(th, -1);
		else if (in == 'i')
		{
			screenshotted = true;
			show_contents(self->pack, ITCAT_ALL);
			continue;
		}
		else if (in == ':')
		{
			screenshot();
			screenshotted = true;
			int k = 0;
			ITER_THINGS(n, num)
			{
				struct Thing *t_ = n->data;
				if (t_->type != THING_ITEM)
					continue;
				if (t_->xloc == th->xloc && t_->yloc == th->yloc)
				{
					char *line =
						get_inv_line(((struct Thing *)(n->data))->thing);
					pline("You%s see here %s. ", ((k++) ? " also" : ""),
						  line);
					free(line);
				}
			}
			if (k == 0)
				pline("You see nothing here. ");
		}
		else if (in == 'w')
		{
		  retry:
			line_reset();
			pline("Wield what?");
			in = gr_getch();
			if (in == ' ')
			{
				line_reset();
				pline("Never mind.");
				continue;
			}
			struct Item *it = get_Itemc(self->pack, in);
			if (it == NULL)
			{
				pline("No such item.");
				goto retry;
			}
			if (mons_unwield(self))
				mons_wield(self, it);
		}
		else
		{
			screenshot();
			screenshotted = true;
			pline("Unknown command '%s%c'. ",
				  (escape(in) == in ? "" : "^"), escape(in));
		}
	}
	return true;
}

void mons_dead(struct Monster *from, struct Monster *to)
{
	if (IS_PLAYER(to))
	{
		player_dead("");
		return;
	}

	event_mkill (from, to);
	if (IS_PLAYER(from))
	{
		if (to->type == MTYP_SATAN)
			U.playing = PLAYER_WONGAME;
		from->exp += mons[to->type].exp;
		update_level(from);
	}
	struct item_struct *c = find_corpse(to);
	struct list_iter *i = get_iter(to);
	struct Thing *t = i->data;
	struct Item *it = malloc(sizeof(struct Item));
	it->type = c;
	it->attr = 0;
	it->name = NULL;
	it->cur_weight = 0;
	new_thing(THING_ITEM, t->yloc, t->xloc, it);
	rem_by_data(to);
	thing_free(t);
}

/* TODO is it polymorphed? */
inline bool mons_edible(struct Monster *self, struct Item *item)
{
	return (item->type->ch == ITEM_FOOD);
}

bool mons_eating(struct Monster * self)
{
	int hunger_loss;
	struct Item *item = self->eating;
	if (!item)
		return false;
	if (item->cur_weight <= 1200)
	{
		if (IS_PLAYER(self))
		{
			U.hunger -= (item->cur_weight) >> 4;	/* U.hunger is signed */
			line_reset();
			pline("You finish eating.");
		}
		self->status &= ~M_EATING;
		self->eating = NULL;
		free(item->type);
		rem_by_data(item);
		self->pack.items[PACK_AT(get_Itref(self->pack, item))] = NULL;
		update_stats();
		return false;
	}
	hunger_loss = RN(25) + 50;
	item->cur_weight -= hunger_loss << 4;
	if (IS_PLAYER(self))
		U.hunger -= hunger_loss;
	return true;
}

void mons_eat(struct Monster *self, struct Item *item)
{
	if (!mons_edible(self, item))
	{
		if (IS_PLAYER(self))
			pline("You can't eat that!");
		return;
	}
	if ((self->status) & M_EATING)
	{
		if (IS_PLAYER(self))
			pline("You're already eating!");
		return;
	}
	self->status |= M_EATING;
	self->eating = item;
	if (!item->cur_weight)
		item->cur_weight = item->type->wt;
}

inline struct Item **mons_get_weap(struct Monster *self)
{
	return &self->wearing.rweap;
}

bool mons_unwield(struct Monster * self)
{
	struct Item **pweap = mons_get_weap(self);
	struct Item *weap = *pweap;
	if (weap == NULL)
		return true;
	if (weap->attr & ITEM_CURS)
	{
		if (IS_PLAYER(self))
		{
			line_reset();
			pline("You can't. It's cursed.");
		}
		return false;
	}
	weap->attr ^= ITEM_WIELDED;
	*pweap = NULL;
	return true;
}

bool mons_wield(struct Monster * self, struct Item * it)
{
	self->wearing.rweap = it;
	it->attr ^= ITEM_WIELDED;
	if (IS_PLAYER(self))
	{
		line_reset();
		item_look(it);
	}
	return true;
}

bool mons_wear(struct Monster * self, struct Item * it)
{
	if (it->type->ch != ITEM_ARMOUR)
	{
		if (IS_PLAYER(self))
		{
			line_reset();
			pline("You can't wear that!");
		}
		return false;
	}
	switch (it->type->type)
	{
	case IT_GLOVES:
		{
			self->wearing.hands = it;
			break;
		}
	default:
		{
			panic("Armour not recognised");
		}
	}
	return true;
}

void mons_passive_attack(struct Monster *self, struct Monster *to)
{
	uint32_t t;
	char *posv;
	for (t = 0; t < A_NUM; ++t)
		if ((mons[self->type].attacks[t][2] & 0xFFFF) == ATTK_PASS)
			break;
	if (t == A_NUM)
		return;
	switch (mons[self->type].attacks[t][2] >> 16)
	{
	case ATYP_ACID:
		{
			posv = malloc(strlen(mons[self->type].name) + 5);
			gram_pos(posv, (char *)mons[self->type].name);
			if (IS_PLAYER(self))
				pline("You splash the %s with your acid!",
					  mons[to->type].name);
			else if (IS_PLAYER(to))
				pline("You are splashed by the %s acid!", posv);
		}
	}
}

int mons_get_st(struct Monster *self)
{
	if (IS_PLAYER(self))
		return U.attr[AB_ST];
	return 1;
}

inline void apply_attack(struct Monster *from, struct Monster *to)
{
	int t, strength;

	for (t = 0; t < A_NUM; ++t)
	{
		if (!mons[from->type].attacks[t][0])
			break;

		switch (mons[from->type].attacks[t][2] & 0xFFFF)
		{
			case ATTK_HIT:
			{
				struct Item **it = mons_get_weap(from);
				if (!it || !(*it))
				{
					strength = RN(mons_get_st(from)) >> 1;
					to->HP -=
						RND(mons[from->type].attacks[t][0],
							mons[from->type].attacks[t][1]) + strength +
						RN(3 * from->level);
				}
				else
				{
					struct item_struct is = *((*it)->type);
					strength = RN(mons_get_st(from)) >> 1;
					to->HP -=
						RND(is.attr & 15, (is.attr >> 4) & 15) + strength;
				}

				mons_passive_attack(to, from);
				break;
			}
			case ATTK_TOUCH:
			{
				to->HP -=
					RND(mons[from->type].attacks[t][0],
						mons[from->type].attacks[t][1]);

				mons_passive_attack(to, from);
				break;
			}
			case ATTK_MAGIC:
			{
				break;
			}
			case ATTK_CLAW:
			{
				to->HP -=
					RND(mons[from->type].attacks[t][0],
						mons[from->type].attacks[t][1]);

				mons_passive_attack(to, from);
				break;
			}
			case ATTK_BITE:
			{
				to->HP -=
					RND(mons[from->type].attacks[t][0],
						mons[from->type].attacks[t][1]);

				mons_passive_attack(to, from);
				break;
			}
		}
		event_mhit (from, to, mons[from->type].attacks[t][2] & 0xFFFF);

		if (to->HP <= 0)
		{
			mons_dead(from, to);
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
	if (!magic_isspell(c))
	{
		pline("Unknown spell '%s%c'. ",
			  (escape(c) == c ? "" : "^"), escape(c));
		return false;
	}

	magic_plspell(get_pmonster(), c);
	return true;
}

/* Rudimentary AI system -- move towards player if player is visible. */
int AI_Attack (int fromy, int fromx, int toy, int tox, struct Monster *monst)
{
	int xmove = 0, ymove = 0;
	bres_start(fromy, fromx, NULL, get_sq_attr());
	if (!bres_draw(toy, tox))
	{
		mons_move(monst, RN(3) - 2, RN(3) - 2);
		return 1;
	}
	if (fromy < toy)
		ymove = 1;
	else if (fromy > toy)
		ymove = -1;
	if (fromx < tox)
		xmove = 1;
	else if (fromx > tox)
		xmove = -1;
	if (!mons_move(monst, ymove, xmove))
		if (!mons_move(monst, ymove, 0))
			if (!mons_move(monst, 0, xmove))
				if (!mons_move(monst, -ymove, xmove))
				{
				}
	return 1;
}
