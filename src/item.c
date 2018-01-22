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

/* Item weight should roughly be in grams - so a stong person (St >= 18) would 
   be able to carry 30000g before getting burdened, and weak would be just
   5000g or so */

int NUM_ITEMS;
struct Item no_item;
Ityp ityp_fireball, ityp_battle_axe, ityp_long_sword, ityp_water_bolt, ityp_ice_bolt;
#define ITEM(nm,tp,wt,attr,gl) {nm,tp,wt,attr,gl}
#define DMG(a,b) (((a)<<4)+(b))

/* No corpse -- they are a custom item (see src/monst.c). */
Ityp all_items[] = {
/*  item name              type            weight  attributes  display                             */
	ITEM("long sword",     ITYP_LONGSWORD,  2000,  DMG(1, 5),  ITCH_WEAPON | COL_TXT(11,11, 0)),
	ITEM("fencing sword",  ITYP_LONGSWORD,  1500,  DMG(1, 5),  ITCH_WEAPON | COL_TXT(11, 0,11)),
	ITEM("axe",            ITYP_AXE,        3000,  DMG(1, 4),  ITCH_WEAPON | COL_TXT(11,11, 0)),
	ITEM("battle-axe",     ITYP_AXE,        1000,  DMG(1, 4),  ITCH_WEAPON | COL_TXT(11,11, 0)),
	ITEM("war hammer",     ITYP_HAMMER,     5000,  DMG(2, 4),  ITCH_WEAPON | COL_TXT(15,11, 0)),
	ITEM("dagger",         ITYP_DAGGER,     100,   DMG(1, 3),  ITCH_WEAPON | COL_TXT( 0,11, 0)),
	ITEM("short sword",    ITYP_SHORTSWORD, 2000,  DMG(1, 4),  ITCH_WEAPON | COL_TXT(11, 8, 0)),
	ITEM("gloves",         ITYP_GLOVES,     70,    0,          ITCH_ARMOUR | COL_TXT( 0,11, 0)),
	ITEM("gold piece",     ITYP_MONEY,      1,     0,          ITCH_DOSH   | COL_TXT(15,15, 0)),
	ITEM("fireball",       ITYP_ARCANE,     0,     0,          0x09        | COL_TXT(15, 4, 0)),
	ITEM("water bolt",     ITYP_ARCANE,     0,     0,          0x07        | COL_TXT( 0, 4,15)),
	ITEM("ice bolt",       ITYP_ARCANE,     0,     0,          0x07        | COL_TXT( 0, 8,15)),
	ITEM("",               ITYP_NONE,       0,     0,          0                                  )
/*  item name              type            weight  attributes  display                             */
};

void ityp_init ()
{
	int i;
	for (i = 0; all_items[i].name[0]; ++ i)
	{
		if (!strcmp(all_items[i].name, "long sword"))
			ityp_long_sword = all_items[i];
		else if (!strcmp(all_items[i].name, "fireball"))
			ityp_fireball = all_items[i];
		else if (!strcmp(all_items[i].name, "battle-axe"))
			ityp_battle_axe = all_items[i];
		else if (!strcmp(all_items[i].name, "water bolt"))
			ityp_water_bolt = all_items[i];
		else if (!strcmp(all_items[i].name, "ice bolt"))
			ityp_ice_bolt = all_items[i];
	}
	NUM_ITEMS = i;
}

char *get_near_desc (const struct Monster *mons, const struct Item *item)
{
	char *ret = malloc(128);
	char temp[128];
	if (item->name != NULL && item->name[0] != '\0')
	{
		p_msg ("NULULULULUL");
	}
	else
	{
		// const char *str = itoa((item->attr&ITEM_PLUS(3)>>3))
		snprintf (temp, 128, "%s%s%s%s",
		         /* beatitude */
		         (!(item->attr & ITEM_KBUC)) ? "" :
		           (item->attr & ITEM_BLES)  ? "blessed " :
		           (item->attr & ITEM_CURS)  ? "cursed "  : "uncursed ",
		         /* greasedness */
		         (item->attr & ITEM_GREASED) ? "greased " : "",
		         /* enchantment value */
		         //((item->attr & (ITEM_MINS(3))) | 1 ? ((item->attr & (ITEM_MINS(0))) ? "-" : "+") : ""),
		         //((item->attr & (ITEM_MINS(3))) | 1 ? (item->attr & ((ITEM_PLUS(3)) >> 3)) : 0),
		         /* name */
		         item->type.name,
		         /* wielded */
		         (item->loc.loc == LOC_WIELDED) ? " (wielded)" : ""
				 );
		w_a (ret, temp, 128);
	}
	return ret;
}

char *get_inv_line (const struct Item *item)
{
	char ch = get_Itref (item);
	char *ret = malloc (80), *orig = get_near_desc (MTHIID(item->loc.inv.monsID), item);
	snprintf (ret, 80, "%c - %s", ch, orig);
	free (orig);
	return ret;
}

bool stackable (int n, Vector *pile, int i)
{
	//struct Item *item1 = &THING(n, *(int*)v_at (*pile, 0))->thing.item,
	//            *item2 = &THING(n, i)->thing.item;
	//return (memcmp (&item1->type, &item2->type, sizeof(Ityp)) == 0);
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

int items_equal (struct Item *it1, struct Item *it2)
{
	if (it1 == it2)
		return 1;
	if (NO_ITEM(it1) && NO_ITEM(it2))
		return 1;
	return 0;
}

