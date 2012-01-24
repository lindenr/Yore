/* monst.c
 * Linden Ralph */

/* MONSTERS */

#include "all.h"
#include "monst.h"
#include "pline.h"
#include "rand.h"
#include "util.h"
#include "mycurses.h"
#include "bool.h"
#include "loop.h"
#include <stdarg.h>

void mons_attack (struct Monster *self, int y, int x) /* each either -1, 0 or 1 */
{
	struct Thing *th = get_thing(self);
    apply_attack(self, get_square_monst(th->yloc+y, th->xloc+x, self->level));
}

int  mons_move (struct Monster *self, int y, int x) /* each either -1, 0 or 1 */
{
    if (self->name[0] != '_')
	if (!(x|y)) return false;
    struct Thing* t = get_thing(self);
    int can = can_move_to(get_square_attr(t->yloc+y, t->xloc+x, self->level));
    /* like a an unmoveable boulder or something */
    if (!can)
        return false;
    /* you can and everything's fine, nothing doing */
    else if (can == 1)
    {
        t->yloc += y;
        t->xloc += x;
        return true;
    }
    /* melee attack! */
    else if (can == 2)
    {
        mons_attack(self, y,x);
        return true;
    }
    /* off map or something */
    else if (can == -1)
    {
        /* nothing to do except return false (move not allowed) */
		return false;
    }
    /* shouldn't get to here
     * been a mistake */
    return false;
}

inline char escape(char a)
{
	if (a < 0x20)
		return a+0x40;
	else
		return a;
}

inline bool mons_take_input(struct Thing *th, char in)
{
	int xmove=0, ymove=0;
	if (in == 'h') xmove = -1;
	else if (in == 'j') ymove = 1;
	else if (in == 'k') ymove = -1;
	else if (in == 'l') xmove = 1;
	else if (in == 'y'){ymove=-1;xmove=-1;}
	else if (in == 'u'){ymove=-1;xmove=1;}
	else if (in == 'n'){ymove=1;xmove=1;}
	else if (in == 'b'){ymove=1;xmove=-1;}
	else return(-1);

	return(mons_move(th->thing, ymove, xmove));
}

int  mons_take_move(struct Monster *self)
{
	struct Thing *th = get_thing(self);
	bool screenshotted = false;
    if (self->name[0] == '_')
    {
        while(1)
        {
			refresh();
			move(th->yloc+1, th->xloc);
            char in = getch();
			if (pline_check()) line_reset();
			if (screenshotted)
			{
				screenshotted = false;
				unscreenshot();
			}
            if (in == 'Q') 
            {
                if (!quit()) return false;
                continue;
            }
			//if (in == 'S') return save();
            
			bool mv = mons_take_input(th, in);
			if (mv != -1)
			{
				if (mv) break;
			}
            else if (in == '.') break;
            else if (in == ',')
            {
                screenshot();
                screenshotted = true;
                struct List Li = {&list_beg, &list_end};
                struct list_iter *li;
                for(li = all_things.beg; iter_good(li); next_iter(&li))
                {
                    struct Thing *t_ = li->data;
                    if (t_->type != THING_ITEM) continue;
                    if (t_->xloc == th->xloc &&
                        t_->yloc == th->yloc)
                    {
                        push_back(&Li, t_);
                    }
                }
                if (Li.beg == Li.end) /* One element in list. */
                {
                    pack_add(&self->pack, ((struct Thing*)(Li.beg->data))->thing);
                    rem_by_data(((struct Thing*)(Li.beg->data))->thing);
                }
            }
            else if (in == 'd')
            {
                char *cs;
                redo:
                cs = pack_get_letters(self->pack);
                in = pask(cs, "Drop what?");
                free(cs);
                if (in == '?') continue;
                if (in == '*')
                {
                    if (!screenshotted)
                    {
                        screenshotted = true;
                        show_contents(self->pack);
                    }
                    goto redo;
                }
                else
                {
                    struct Item *it = pack_rem(&self->pack, in);
                    new_thing(THING_ITEM, th->yloc, th->xloc, it);
                }
            }
            else if (in == 'i')
            {
                screenshotted = true;
                show_contents(self->pack);
                continue;
            }
            else if (in == ':')
            {
                screenshot();
                screenshotted = true;
                struct list_iter *n;
                int k = 0;
                for(n = all_things.beg; iter_good(n); next_iter(&n))
				{
                    struct Thing *t_ = n->data;
					if (t_->type != THING_ITEM) continue;
                    if (t_->xloc == th->xloc &&
					    t_->yloc == th->yloc)
				    {
                        pline("You%s see here %s. ", ((k++==0)?"":" also"), get_inv_line(((struct Thing*)(n->data))->thing));
                    }
				}
				if (k == 0) pline("You see nothing here. ");
				continue;
			}
			else if (in == 'w')
			{
				retry:
				pline("Wield what?");
				in = getch();
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
				pline ("Unknown command '%s%c'. ", (escape(in)==in?"":"^"), escape(in));
                continue;
            }
        }
    }
    else
    {
		struct Thing *pl = get_player();
        AI_Attack(th->yloc, th->xloc, pl->yloc, pl->xloc, self);
    }
    return true;
}

void mons_dead(struct Monster *from, struct Monster* to)
{
	if (to->name[0] == '_')
	{
		player_dead("");
		return;
	}
    if (from->name[0] == '_')
        pline("You kill the %s!", mons[to->type].name);
    else
        pline("The %s dies...", mons[to->type].name);
    uint32_t u = find_corpse(mons[to->type].name);
    if (u != -1)
    {
        struct list_iter *i = get_iter(to);
        struct Thing *t = i->data;
        struct Item *it = malloc(sizeof(struct Item));
        it->type = u;
        it->attr = 0;
        it->name = NULL;
        new_thing(THING_ITEM, t->yloc, t->xloc, it);
    }
    rem_by_data(to);
}

inline struct Item **mons_get_weap(struct Monster *self)
{
	return &self->wearing.rweap;
}

bool mons_unwield(struct Monster *self)
{
	struct Item **pweap = mons_get_weap(self);
	struct Item *weap = *pweap;
	if (weap == NULL) return true;
	if (weap->attr&ITEM_CURS)
	{
		if (self->name[0] == '_')
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

bool mons_wield(struct Monster *self, struct Item *it)
{
	self->wearing.rweap = it;
	it->attr ^= ITEM_WIELDED;
	if (self->name[0] == '_')
	{
		line_reset();
		item_look(it);
	}
	return true;
}

bool mons_wear(struct Monster *self, struct Item *it)
{
	if(items[it->type].ch != ITEM_ARMOUR)
	{
		if (self->name[0] == '_')
		{
			line_reset();
			pline("You can't wear that!");
		}
		return false;
	}
	switch(it->type)
	{
		case IT_GLOVES:
		{
			self->wearing.hands = it;
			break;
		}
		default:
		{
#define DEBUGGING
#if defined(DEBUGGING)
			pline("_ERR: "ARMOUR" not recognised: %s", items[it->type].name);
#endif /* DEBUGGING */
		}
	}
	/* message */
}

#define COL(cl) cl
#define MONST(nm,ch,sp,at,cl) {nm,ch,sp,at,COL(cl)}
#define ATTK(a1,a2,a3,a4,a5,a6) {a1,a2,a3,a4,a5,a6}
#define A(a,b,c) {a,b,c}
#define AT_NONE {0,0,0}

const struct monster_struct mons[] = {
MONST("gnome", 'G', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_RED|COL_TXT_GREEN),
MONST("human", '@', 12,
      ATTK(A(1,4,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_BLUE|COL_TXT_RED|COL_TXT_GREEN),
MONST("dwarf", 'h', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_RED),
MONST("hobbit", 'h', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_GREEN),
MONST("lich", 'L', 8,
      ATTK(A(2,4,ATTK_HIT), A(2,2,ATTK_MAGIC), AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_RED|COL_TXT_BLUE)
};

inline void apply_attack(struct Monster *from, struct Monster *to)
{
	uint32_t t;

    for (t = 0; t < A_NUM; ++ t)
	{
        if (!mons[from->type].attacks[t][0]) break;
		switch(mons[from->type].attacks[t][2])
		{
			case ATTK_HIT:
			{
				struct Item **it = mons_get_weap(from);
				if (!it || !(*it))
				{
                    to->HP -= RN(from->attr[AB_ST]);
				    if (from->name[0] == '_') pline("You hit the %s!", mons[to->type].name);
				    else pline("The %s hits you!", mons[from->type].name);
                    break;
				}
				struct item_struct is = items[(*it)->type];
				to->HP -= RND(is.attr&15, (is.attr>>4)&15);
				if (from->name[0] == '_') pline("You smite the %s!", mons[to->type].name);
				else pline("The %s hits you!", mons[from->type].name);
				break;
			}
			case ATTK_TOUCH:
			{
				to->HP -= RND(mons[from->type].attacks[t][0], mons[from->type].attacks[t][1]);
				if (from->name[0] == '_') pline("You touch the %s!", mons[to->type].name);
				else pline("The %s touches you!", mons[from->type].name);
				break;
			}
			case ATTK_PASS:
			{pline("Passive attack not implemented");
				break;
			}
			case ATTK_MAGIC:
			{pline("Magic attack not implemented");
				break;
			}
		}
	}
    if (to->HP <= 0) mons_dead(from, to);
}

void player_dead(const char *msg, ...)
{
	va_list args;
	char *actual = malloc(sizeof(char)*80);
	
	va_start(args, msg);
	if (msg == "") msg = "You die...";
	vsprintf(actual, msg, args);
	line_reset();
	pline(actual);
	getch();
	va_end(args);
}

/* END MONSTERS */


/* AI */

int AI_Attack(int fromy, int fromx, int toy, int tox, struct Monster *monst)
{
    int xmove = 0, ymove = 0;
    if (fromy<toy) ymove = 1;
    else if (fromy>toy) ymove = -1;
    if (fromx<tox) xmove = 1;
    else if (fromx>tox) xmove = -1;
    if (!mons_move(monst, ymove, xmove))
		if (!mons_move(monst, ymove, 0))
			if (!mons_move(monst, 0, xmove)) {}
    return 1;
}

/* END AI (in MONSTERS) */
