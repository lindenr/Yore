/* item.c */

#include "include/all.h"
#include "include/item.h"
#include "include/grammar.h"
#include "include/monst.h"
#include "include/pline.h"
#include "include/vector.h"

#include <stdio.h>
#include <malloc.h>

/* Item weight should roughly be in grams - so a stong person (St >= 18) would 
   be able to carry 30000g before getting burdened, and weak would be just
   5000g or so */

int NUM_ITEMS;
#define ITEM(nm,ch,tp,wt,attr,col) {nm,ch,tp,wt,attr,col}
#define DMG(a,b) (((a)<<4)+(b))

/* No corpse -- they are a custom item (see src/monst.c). */
struct item_struct items[] = {
	/* item name             display         type     weight  attributes           colour */
	ITEM("long sword",     ITEM_WEAPON, IT_LONGSWORD,  2000,  DMG(1, 5),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("fencing sword",  ITEM_WEAPON, IT_LONGSWORD,  1500,  DMG(1, 5),  COL_TXT_RED(11)   | COL_TXT_BLUE(11) ),
	ITEM("axe",            ITEM_WEAPON, IT_AXE,        3000,  DMG(1, 4),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("battle-axe",     ITEM_WEAPON, IT_AXE,        5000,  DMG(1, 7),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("dagger",         ITEM_WEAPON, IT_DAGGER,     100,   DMG(1, 3),  COL_TXT_GREEN(11)                    ),
	ITEM("short sword",    ITEM_WEAPON, IT_SHORTSWORD, 2000,  DMG(1, 4),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("gloves",         ITEM_ARMOUR, IT_GLOVES,     70,    0,          COL_TXT_GREEN(11)                    ),
	ITEM("gold",           ITEM_DOSH,   IT_MONEY,      1,     0,          COL_TXT_RED(15)   | COL_TXT_GREEN(15)),
	ITEM("water charm",    ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_BLUE(11)                     ),
	ITEM("air charm",      ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_GREEN(15)                    ),
	ITEM("fire charm",     ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_RED(15)                      ),
	ITEM("volt charm",     ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_BLUE(15)  | COL_TXT_RED(15)  ),
	ITEM("ice charm",      ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_BLUE(15)                     ),
	ITEM("earth charm",    ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_GREEN(11) | COL_TXT_RED(11)  ),
	ITEM("jewel",          ITEM_JEWEL,  IT_JEWEL,      100,   0,          COL_TXT_BRIGHT                       ),
	ITEM("",               0,           0,             0,     0,          0                                    )
	/* item name             display         type     weight  attributes           colour */
};

char *get_item_desc (struct Item item)
{
	char *ret = malloc(80);
	if (item.name != NULL && item.name[0] != '\0')
	{
		pline("NULULULULUL");
	}
	else
	{
		// const char *str = itoa((item.attr&ITEM_PLUS(3)>>3))
		sprintf(ret, "%s%s%s%d %s%s", ((item.attr & ITEM_KBUC) ? ((item.attr & ITEM_BLES) ? "blessed " : (item.attr & ITEM_CURS) ? "cursed " : "uncursed ") : ""), /* BUC */
				/* greasedness */
				((item.attr & ITEM_GREASED) ? "greased " : ""),
				/* enchantment value */
				((item.
				  attr & (ITEM_MINS(3))) | 1 ? ((item.
												 attr & (ITEM_MINS(0))) ? "-" :
												"+") : ""),
				((item.attr & (ITEM_MINS(3))) | 1 ? (item.
													 attr & ((ITEM_PLUS(3)) >>
															 3)) : 0),
				item.type->name,
				(item.attr & ITEM_WIELDED ? " (being used)" : ""));
		gram_a(ret, ret);
	}
	return ret;
}

void item_look (struct Item *item)
{
	pline ("%s", get_inv_line(item));
}

char *get_inv_line (struct Item *item)
{
	struct Monster *m = (get_player()->thing);
	char ch = get_Itref(m->pack, item);
	char *ret = malloc (sizeof(char) * 80), *orig = get_item_desc(*item);
	if (!ch)
		sprintf (ret, "%s", orig);
	else
		sprintf (ret, "%c - %s", ch, orig);
	free (orig);
	return ret;
}

bool stackable (Vector pile, struct Item *it)
{
	struct Item *pitem = pile->data[0];
	return (memcmp (pitem->type, it->type, sizeof(struct item_struct)) == 0);
}

void item_piles (Vector piles, Vector items)
{
	int i;
	for (i = 0; i < items->len; ++ i)
	{
		int j;
		for (j = 0; j < piles->len; ++ j)
		{
			if (stackable (piles->data[j], piles->data[i]))
				break;
		}

		if (j < piles->len) /* break'd */
		{
			v_push (piles->data[j], piles->data[i]);
		}
		else
		{
			piles->data[j] = v_dinit ();
			v_push (piles->data[j], piles->data[i]);
		}
	}
}

void spell_simulator ()
{
}

void what_am_I_wearing (struct Monster *self)
{
	return;
}
