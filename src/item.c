/* item.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/words.h"
#include "include/monst.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/item.h"
#include "include/dlevel.h"

#include <stdio.h>
#include <malloc.h>

/* Item weight should roughly be in grams - so a stong person (St >= 18) would 
   be able to carry 30000g before getting burdened, and weak would be just
   5000g or so */

int NUM_ITEMS;
#define ITEM(nm,ch,tp,wt,attr,col) {nm,ch,tp,wt,attr,col}
#define DMG(a,b) (((a)<<4)+(b))

/* No corpse -- they are a custom item (see src/monst.c). */
ityp items[] = {
	/* item name             display         type     weight  attributes           colour */
	ITEM("long sword",     ITEM_WEAPON, IT_LONGSWORD,  2000,  DMG(1, 5),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("fencing sword",  ITEM_WEAPON, IT_LONGSWORD,  1500,  DMG(1, 5),  COL_TXT_RED(11)   | COL_TXT_BLUE(11) ),
	ITEM("axe",            ITEM_WEAPON, IT_AXE,        3000,  DMG(1, 4),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("battle-axe",     ITEM_WEAPON, IT_AXE,        5000,  DMG(5, 7),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("dagger",         ITEM_WEAPON, IT_DAGGER,     100,   DMG(1, 3),  COL_TXT_GREEN(11)                    ),
	ITEM("short sword",    ITEM_WEAPON, IT_SHORTSWORD, 2000,  DMG(1, 4),  COL_TXT_RED(11)   | COL_TXT_GREEN(11)),
	ITEM("gloves",         ITEM_ARMOUR, IT_GLOVES,     70,    0,          COL_TXT_GREEN(11)                    ),
	ITEM("gold piece",     ITEM_DOSH,   IT_MONEY,      1,     0,          COL_TXT_RED(15)   | COL_TXT_GREEN(15)),
/*	ITEM("water charm",    ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_BLUE(11)                     ),
	ITEM("air charm",      ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_GREEN(15)                    ),
	ITEM("fire charm",     ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_RED(15)                      ),
	ITEM("volt charm",     ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_BLUE(15)  | COL_TXT_RED(15)  ),
	ITEM("ice charm",      ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_BLUE(15)                     ),
	ITEM("earth charm",    ITEM_CHARM,  IT_CHARM,      100,   0,          COL_TXT_GREEN(11) | COL_TXT_RED(11)  ),
	ITEM("jewel",          ITEM_JEWEL,  IT_JEWEL,      100,   0,          COL_TXT_BRIGHT                       ),*/
	ITEM("",               0,           0,             0,     0,          0                                    )
	/* item name             display         type     weight  attributes           colour */
};

void ask_items (Vector it_out, Vector it_in, const char *msg)
{
	int i;
	// TODO fix
	Vector list = v_init (128, it_in->len);
	char first[256];
	snprintf (first, 128, " ## %s ##",  msg);
	v_pstr (list, first);
	v_pstr (list, "");
	for (i = 0; i < it_in->len; ++ i)
	{
		v_push (it_out, v_at (it_in, i));
		char *asdf = get_item_desc ((*(struct Thing **)v_at(all_ids, *(int*)v_at(it_in, i)))->thing.item);
		v_pstrf (list, "  (*)   %s", asdf);
		free (asdf);
	}
	v_pstr (list, "");
	p_lines (list);
	v_free (list);
}

char *get_item_desc (const struct Item item)
{
	char *ret = malloc(128);
	char temp[128];
	if (item.name != NULL && item.name[0] != '\0')
	{
		p_msg ("NULULULULUL");
	}
	else
	{
		// const char *str = itoa((item.attr&ITEM_PLUS(3)>>3))
		snprintf (temp, 128, "%s%s %s%s",
		         /* beatitude */
		         (!(item.attr & ITEM_KBUC)) ? "" :
		          (item.attr & ITEM_BLES) ? "blessed " :
		           (item.attr & ITEM_CURS) ? "cursed " : "uncursed ",
		         /* greasedness */
		         (item.attr & ITEM_GREASED) ? "greased " : "",
		         /* enchantment value */
		         //((item.attr & (ITEM_MINS(3))) | 1 ? ((item.attr & (ITEM_MINS(0))) ? "-" : "+") : ""),
		         //((item.attr & (ITEM_MINS(3))) | 1 ? (item.attr & ((ITEM_PLUS(3)) >> 3)) : 0),
		         /* name */
		         item.type.name,
		         /* wielded */
		         item.attr & ITEM_WIELDED ? " (wielded)" : "");
		w_a (ret, temp, 128);
	}
	return ret;
}

void item_look (const struct Item *item)
{
	char *str = get_inv_line (item);
	p_msg ("%s", str);
	free (str);
}

char *get_inv_line (const struct Item *item)
{
	char ch = get_Itref (pmons.pack, item);
	char *ret = malloc (80), *orig = get_item_desc(*item);
	if (!ch)
		snprintf (ret, 80, "%s", orig);
	else
		snprintf (ret, 80, "%c - %s", ch, orig);
	free (orig);
	return ret;
}

bool stackable (int n, Vector *pile, int i)
{
	//struct Item *item1 = &THING(n, *(int*)v_at (*pile, 0))->thing.item,
	//            *item2 = &THING(n, i)->thing.item;
	//return (memcmp (&item1->type, &item2->type, sizeof(ityp)) == 0);
	return false;
}

void item_piles (int n, Vector piles, Vector items)
{
	int i;
	for (i = 0; i < items->len; ++ i)
	{
		int j;
		for (j = 0; j < piles->len; ++ j)
		{
			if (stackable (n, v_at (piles, j), *(int*)v_at (items, i)))
				break;
		}

		if (j >= piles->len)
			*(Vector*)v_at (piles, j) = v_dinit (sizeof(int));

		v_push (*(Vector*)v_at (piles, j), v_at (items, i));
	}
}

void what_am_I_wearing (struct Monster *self)
{
	return;
}
