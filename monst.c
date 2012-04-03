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
#include "save.h"
#include <stdarg.h>

struct player_status U;
char *s_hun[] = {
    "Full   ",
    "       ",
    "Hungry ",
    "Hungry!",
    "Starved",
    "Dead   "};

char *get_hungerstr()
{
    if (U.hunger <  100) return s_hun[0];
    if (U.hunger <  125) return s_hun[1];
    if (U.hunger <  500) return s_hun[2];
    if (U.hunger <  730) return s_hun[3];
    if (U.hunger < 1000) return s_hun[4];
    return s_hun[5];
}

bool digesting()
{
    return true;
}

void setup_U()
{
    U.hunger = 100;
    U.playing = PLAYER_STARTING;
}

void get_cinfo()
{
    char in;

    mvprintw(0, 0, "What role would you like to take up?");
    move(0, console_width-11);
    fprintf(stdout, "(q to quit)");
    mvprintw(2, 0, "[d] --- Doctor");
    mvprintw(3, 0, "[s] --- Soldier");

    do
    {
        in = getch();
        if (in == 'q' || in == 0x1B) return;
    }
    while (in != 'd' && in != 's');
    
    if (in == 'd') U.role = 2;
    else if (in == 's') U.role = 1;

    move(0, console_width-11);
    fprintf(stdout, "           ");

    U.playing = PLAYER_PLAYING;
}

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

void thing_move_level(struct Thing *th, int32_t where)
{
    uint32_t wh;
    if(where == 0) /* Uncontrolled teleport within level */
    {
        do wh = RN(1680);
        while(!is_safe_gen(wh/80, wh%80));
        th->yloc = wh/80;
        th->xloc = wh%80;
    }
    else if (where == 1) /* go up stairs */
    {
    }
    else if (where == -1) /* go down stairs */
    {
    }
    else /* levelport -- always uncontrolled, see thing_move_level_c(TODO) for controlled teleportation */
    {
        where >>= 1; /* LSB is unused */
    }
}

struct Item *player_use_pack(struct Thing *player, char *msg, bool *psc)
{
    struct Item *It;
    char in, cs[MAX_ITEMS_IN_PACK+4];
    struct Monster *self = player->thing;

    redo:
    line_reset();
    pack_get_letters(self->pack, cs);
    in = pask(cs, msg);
    if (in == '?') return NULL; // TODO change
    if (in == ' ' || in == 0x1B) return NULL;
    if (in == '*')
    {
        if (!*psc)
        {
            *psc = true;
            show_contents(self->pack);
        }
        goto redo;
    }

    It = get_Itemc(self->pack, in);
    if (It == NULL)
    {
        pline("No such item.");
        goto redo;
    }
    return It;
}

int  mons_take_move(struct Monster *self)
{
    if(self->HP < self->HP_max && RN(50) < self->attr[AB_CO]) self->HP += (self->level+10)/10;
    if (mons_eating(self)) return true;
    struct Thing *th = get_thing(self);
    bool screenshotted = false;
    char *cs; /* just a temp variable */
    if (self->name[0] == '_')
    {
        while(1)
        {
            if (mons_eating(self)) return true;
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
                if (!quit())
                {
                    U.playing = PLAYER_LOSTGAME;
                    return false;
                }
                continue;
            }
            if (in == 'S')
            {
                if (!save())
                {
                    U.playing = PLAYER_SAVEGAME;
                    return false;
                }
                continue;
            }
            
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
                else
                {
                    // TODO add support for multiple items
                }
            }
            else if (in == 'e')
            {
                struct Item *It = player_use_pack(th, "Eat what?", &screenshotted);
                if (It == NULL) continue;
                mons_eat(self, It);
            }
            else if (in == 'd')
            {
                struct Item *It = player_use_pack(th, "Drop what?", &screenshotted);
                if (It == NULL) continue;
                unsigned u = get_Itref(self->pack, It);
                self->pack.items[u] = NULL;
                new_thing(THING_ITEM, th->yloc, th->xloc, It);
                /*drop_redo:
                pack_get_letters(self->pack, cs);
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
                    goto drop_redo;
                }
                else
                {
                    struct Item *it = pack_rem(&self->pack, in);
                    new_thing(THING_ITEM, th->yloc, th->xloc, it);
                }*/
            }
            else if (in == '>') thing_move_level(th, 0);
            else if (in == '<') thing_move_level(th, -1);
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
                        pline("You%s see here %s. ",
                              ((k++)?" also":""),
                              get_inv_line(((struct Thing*)(n->data))->thing));
                    }
                }
                if (k == 0) pline("You see nothing here. ");
                continue;
            }
            else if (in == 'w')
            {
                retry:
                line_reset();
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
        pline("The %s kills the %s!", mons[from->type].name, mons[to->type].name);
    uint32_t u = find_corpse(mons[to->type].name);
    if (u != -1)
    {
        struct list_iter *i = get_iter(to);
        struct Thing *t = i->data;
        struct Item *it = malloc(sizeof(struct Item));
        it->type = u;
        it->attr = 0;
        it->name = NULL;
        it->cur_weight = 0;
        new_thing(THING_ITEM, t->yloc, t->xloc, it);
    }
    rem_by_data(to);
}

/* TODO is it polymorphed? */
inline bool mons_edible(struct Monster *self, struct Item *item)
{
    return (items[item->type].ch == ITEM_FOOD);
}

bool mons_eating(struct Monster *self)
{
    int hunger_loss;
    struct Item *item = self->eating;
    if (!item) return false;
    if (item->cur_weight < 500)
    {
        if (self->name[0] == '_')
        {
            if (U.hunger > (item->cur_weight>>3)) U.hunger -= item->cur_weight>>3;
            else U.hunger = 0; /* death by overeating */
            pline("You finish eating.");
        }
        self->status &= ~M_EATING;
        self->eating = NULL;
        rem_by_data(item);
        self->pack.items[get_Itref(self->pack, item)] = NULL;
        return false;
    }
    hunger_loss = RN(50) + 200;
    //pline("B4: %d", item->cur_weight);
    item->cur_weight -= hunger_loss<<1;
    //pline("After: %d", item->cur_weight);
    if (self->name[0] == '_') U.hunger -= hunger_loss>>2;
    return true;
}

void mons_eat(struct Monster *self, struct Item *item)
{
    if (!mons_edible(self, item)) 
    {
        if (self->name[0] == '_') pline("You can't eat that!");
        return;
    }
    if ((self->status)&M_EATING)
    {
        if (self->name[0] == '_') pline("You're already eating!");
        return;
    }
    self->status |= M_EATING;
    self->eating = item;
    if (!item->cur_weight)
        item->cur_weight = items[item->type].wt;
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
            pline("_ERR: " ARMOUR " not recognised: %s", items[it->type].name);
#endif /* DEBUGGING */
        }
    }
    /* message */
}

#include "all_mon.h"

void mons_passive_attack (struct Monster *self, struct Monster *to)
{
    uint32_t t;
    char *posv;
    for (t = 0; t < A_NUM; ++ t)
        if ((mons[self->type].attacks[t][2]&0xFFFF) == ATTK_PASS) break;
    if (t == A_NUM) return;
    switch(mons[self->type].attacks[t][2]>>16)
    {
        case ATYP_ACID:
        {
            posv = malloc(strlen(mons[self->type].name)+5);
            gram_pos(posv, mons[self->type].name);
            if (self->name[0] == '_') pline("You splash the %s with your acid!", mons[to->type].name);
            else if (to->name[0] == '_') pline("You are splashed by the %s acid!", posv);
        }
    }
}

inline void apply_attack(struct Monster *from, struct Monster *to)
{
    uint32_t t;
    char ton[128];

    for (t = 0; t < A_NUM; ++ t)
    {
        if (!mons[from->type].attacks[t][0]) break;
        switch(mons[from->type].attacks[t][2]&0xFFFF)
        {
            case ATTK_HIT:
            {
                struct Item **it = mons_get_weap(from);
                if (!it || !(*it))
                {
                    to->HP -= RN(from->attr[AB_ST]);
                    if (from->name[0] == '_') pline("You hit the %s!", mons[to->type].name);
                    else pline("The %s hits %s!", mons[from->type].name,
                               to->name[0]=='_'?"you":(gram_the(ton, mons[to->type].name), ton)); 
                    mons_passive_attack (to, from);
                    break;
                }
                struct item_struct is = items[(*it)->type];
                to->HP -= RND(is.attr&15, (is.attr>>4)&15);
                if (from->name[0] == '_') pline("You smite the %s!", mons[to->type].name);
                else pline("The %s hits %s!", mons[from->type].name,
                           to->name[0]=='_'?"you":(gram_the(ton, mons[to->type].name), ton));
                mons_passive_attack (to, from);
                break;
            }
            case ATTK_TOUCH:
            {
                to->HP -= RND(mons[from->type].attacks[t][0], mons[from->type].attacks[t][1]);
                if (from->name[0] == '_') pline("You touch the %s!", mons[to->type].name);
                else if (to->name[0] == '_') pline("The %s touches you!", mons[from->type].name);
                mons_passive_attack (to, from);
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
    bres_start(fromy, fromx, NULL, get_sq_attr());
    if (!bres_draw(toy, tox))
    {
        mons_move(monst, RN(3)-2, RN(3)-2);
        return;
    }
    if (fromy<toy) ymove = 1;
    else if (fromy>toy) ymove = -1;
    if (fromx<tox) xmove = 1;
    else if (fromx>tox) xmove = -1;
    if (!mons_move(monst, ymove, xmove))
        if (!mons_move(monst, ymove, 0))
            if (!mons_move(monst, 0, xmove))
                if (!mons_move(monst, -ymove, xmove)){}
    return 1;
}

/* END AI (in MONSTERS) */
