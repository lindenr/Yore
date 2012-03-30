/* item.c
 * Linden Ralph */

#include "all.h"
#include "item.h"
#include "grammar.h"
#include "monst.h"
#include <stdio.h>

/* Item weight should roughly be in grams - so a stong person (St >= 18) would be able to
 * carry 30000g before getting burdened, and weak would be just 5000g or so */

#define ITEM(nm,ch,tp,wt,attr,col) {nm,ch,tp,wt,attr,col}
#define DMG(a,b) (((a)<<4)+(b))

struct item_struct items[] = {
/*     item name              display      type       weight attributes     colour (default=0)    */
ITEM("long sword",         ITEM_WEAPON,  IT_LONGSWORD, 2000, DMG(1, 5), COL_TXT_RED|COL_TXT_GREEN),
ITEM("fencing sword",      ITEM_WEAPON,  IT_LONGSWORD, 1500, DMG(1, 5), COL_TXT_RED|COL_TXT_BLUE),
ITEM("axe",                ITEM_WEAPON,        IT_AXE, 3000, DMG(1, 4), COL_TXT_RED|COL_TXT_GREEN),
ITEM("battle-axe",         ITEM_WEAPON,        IT_AXE, 5000, DMG(1, 7), COL_TXT_RED|COL_TXT_GREEN),
ITEM("dagger",             ITEM_WEAPON,     IT_DAGGER,  100, DMG(1, 3), COL_TXT_GREEN),
ITEM("short sword",        ITEM_WEAPON, IT_SHORTSWORD, 2000, DMG(1, 4), COL_TXT_RED|COL_TXT_GREEN),
ITEM("gloves",             ITEM_ARMOUR,     IT_GLOVES,   70,     0    , COL_TXT_GREEN),
ITEM("lich corpse",        ITEM_CORPSE,     IT_CORPSE,  600,     0    , COL_TXT_BLUE),
ITEM("hobbit corpse",      ITEM_CORPSE,     IT_CORPSE,  600,     0    , COL_TXT_GREEN),
ITEM("gnome corpse",       ITEM_CORPSE,     IT_CORPSE,  600,     0    , COL_TXT_RED|COL_TXT_GREEN),
ITEM("acid blob corpse",   ITEM_CORPSE,     IT_CORPSE,  600,     0    , COL_TXT_GREEN),
ITEM("gold",                 ITEM_DOSH,      IT_MONEY,    1,     0    , COL_TXT_RED|COL_TXT_GREEN|COL_TXT_BRIGHT),
ITEM(      "",            0,           0,            0,     0    ,      0      )
/*     item name          display     type       weight attributes     colour (default=0)    */
};

uint32_t find_corpse(const char *m)
{
    char cps[30];
    uint32_t u, i;
    strcpy(cps, m);
    strcat(cps, " corpse");
    for (u = 0; *items[u].name; ++ u)
    {
        if (items[u].type != IT_CORPSE) continue;
        if (strcmp(cps, items[u].name) == 0) return u;
    }
    return -1;
}

char *get_item_desc(struct Item item)
{
	char *ret = malloc(sizeof(char)*80);
	if (item.name != NULL && item.name[0] != '\0')
	{
        pline ("NULULULULUL");
	}
	else
	{
		//const char *str = itoa((item.attr&ITEM_PLUS(3)>>3))
		sprintf(ret, "%s%s%s%d %s%s",
				((item.attr&ITEM_KBUC)?((item.attr&ITEM_BLES)?"blessed "
															:(item.attr&ITEM_CURS)?"cursed ":"uncursed ")
									  :""),                               /* BUC */
				((item.attr&ITEM_GREASED)?"greased ":""),                 /* "greased" or "" */
				((item.attr&ITEM_MINS(3))|1?((item.attr&ITEM_MINS(0))?"-":"+"):""), /* '+' or '-' */
				((item.attr&ITEM_MINS(3))|1?(item.attr&ITEM_PLUS(3)>>3):0),         /* '1', '2', or '3' */
				items[item.type].name,
				(item.attr&ITEM_WIELDED?" (being used)":""));
        gram_a(ret, ret);
	}
	return ret;
}

void item_look(struct Item *item)
{
	pline("%s", get_inv_line(item));
}

char *get_inv_line(struct Item *item)
{
	struct Monster *m = (get_player()->thing);
	char ch = get_Itref(m->pack,  item);
	char *ret = malloc(sizeof(char)*80);
	if (!ch)
		sprintf(ret, "%s", get_item_desc(*item));
	else
		sprintf(ret, "%c - %s", ch, get_item_desc(*item));
	return ret;
}

void what_am_I_wearing(struct Monster *self)
{
	return;
}
