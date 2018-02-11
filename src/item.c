/* item.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/words.h"
#include "include/monst.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/item.h"
#include "include/dlevel.h"
#include "include/rand.h"

#include <stdio.h>

/* Item weight should roughly be in grams - so a stong person (St >= 18) would 
   be able to carry 30000g before getting burdened, and weak would be just
   5000g or so */

struct Item no_item;
#define ITYP(nm,tp,wt,attk,def,gl,st) {nm,tp,wt,attk,def,gl,st}
#define DMG(a,b) (((a)<<4)+(b))

/* No corpse -- they are a custom item (see src/monst.c). */
Ityp ityps[] = {
/*  item name              type              weight attributes     display                      */
	ITYP("long sword",     ITSORT_LONGSWORD,  2000,  10, 0,      ITCH_WEAPON | COL_TXT(11,11, 0), 0),
	ITYP("fencing sword",  ITSORT_LONGSWORD,  1500,  10, 0,      ITCH_WEAPON | COL_TXT(11, 0,11), 0),
	ITYP("axe",            ITSORT_AXE,        3000,  8,  0,      ITCH_WEAPON | COL_TXT(11,11, 0), 0),
	ITYP("battle-axe",     ITSORT_AXE,        1000,  16, 0,      ITCH_WEAPON | COL_TXT(11,11, 0), 0),
	ITYP("war hammer",     ITSORT_HAMMER,     5000,  16, 0,      ITCH_WEAPON | COL_TXT(15,11, 0), 0),
	ITYP("dagger",         ITSORT_DAGGER,     100,   6,  0,      ITCH_WEAPON | COL_TXT( 0,11, 0), 0),
	ITYP("short sword",    ITSORT_SHORTSWORD, 2000,  8,  0,      ITCH_WEAPON | COL_TXT(11, 8, 0), 0),
	ITYP("glove",          ITSORT_GLOVE,      70,    0,  1,      ITCH_ARMOUR | COL_TXT( 0,11, 0), 0),
	ITYP("chain mail",     ITSORT_MAIL,       5000,  0,  8,      ITCH_ARMOUR | COL_TXT( 8, 8, 8), 0),
	ITYP("plate mail",     ITSORT_MAIL,       8000,  0,  9,      ITCH_ARMOUR | COL_TXT( 8,11, 0), 0),
	ITYP("helmet",         ITSORT_HELM,       2000,  0,  3,      ITCH_ARMOUR | COL_TXT(11,11, 2), 0),
	ITYP("gold piece",     ITSORT_MONEY,      1,     0,  0,      ITCH_DOSH   | COL_TXT(15,15, 0), 1),
	ITYP("bone",           ITSORT_BONE,       100,   0,  0,      ITCH_CORPSE | COL_TXT(15,15,15), 1),
	ITYP("fireball",       ITSORT_ARCANE,     0,     0,  0,      0x09        | COL_TXT(15, 4, 0), 0),
	ITYP("water bolt",     ITSORT_ARCANE,     0,     0,  0,      0x07        | COL_TXT( 0, 8,15), 0),
	ITYP("ice bolt",       ITSORT_ARCANE,     0,     0,  0,      0x07        | COL_TXT( 0, 15,15), 0),
	ITYP("",               ITSORT_NONE,       0,     0,  0,      0                              , 0)
/*  item name              type              weight attributes     display                      */
};

void ityp_init ()
{
	return;
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
		char ench_string[20] = "";
		if (item->attk)
			snprintf (ench_string, 20, " (%d dmg)", item->attk);
		else if (item->def)
			snprintf (ench_string, 20, " (%d def)", item->def);
		snprintf (temp, 128, "%s%s%s%s%s%s%s",
		         /* beatitude */
		         (!(item->attr & ITEM_KBUC)) ? "" :
		           (item->attr & ITEM_BLES)  ? "blessed " :
		           (item->attr & ITEM_CURS)  ? "cursed "  : "uncursed ",
		         /* greasedness */
		         (item->attr & ITEM_GREASED) ? "greased " : "",
		         /* name */
				 item->type.name,
		         item->stacksize == 1 ? "" : "s",
		         /* enchantment value */
				 ench_string,
		         /* wielded */
		         (item->loc.loc == LOC_WIELDED) ? " (wielded)" : "",
				 item_worn(item) ? " (being worn)" : ""
				 );
		w_some (ret, temp, item->stacksize, 128);
	}
	return ret;
}

int it_can_merge (struct Monster *player, struct Item *item1, struct Item *item2)
{
	return NO_ITEM(item1) || NO_ITEM(item2) ||
		(item1->type.stackable &&
		 (!memcmp(&item1->type, &item2->type, sizeof(Ityp))) &&
		 item1->name == item2->name &&
		 item1->attr == item2->attr &&
		 item1->attk == item2->attk &&
		 item1->def  == item2->def);
}

char *get_inv_line (const struct Item *item)
{
	char ch = get_Itref (item);
	char *ret = malloc (80), *orig = get_near_desc (MTHIID(item->loc.inv.monsID), item);
	snprintf (ret, 80, "%c - %s", ch, orig);
	free (orig);
	return ret;
}

void item_gen (union ItemLoc loc)
{
	struct Item item;
	enum ITEM_TYPE typ = rn(12);
	if (typ == ITYP_GOLD_PIECE)
	{
		int stacksize = rn(50)+30;
		item = new_items (ityps[typ], stacksize);
	}
	else
		item = new_item (ityps[typ]);
	item_put (&item, loc);
}

int items_equal (struct Item *it1, struct Item *it2)
{
	if (it1 == it2)
		return 1;
	if (NO_ITEM(it1) && NO_ITEM(it2))
		return 1;
	return 0;
}

