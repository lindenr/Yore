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
#include "include/skills.h"
#include "include/event.h"

#include <stdio.h>

/* Item weight should roughly be in grams - so a stong person (St >= 18) would 
   be able to carry 30000g before getting burdened, and weak would be just
   5000g or so */

#define ITYP(nm,tp,wt,attk,def,gl,st,fl) {nm,tp,wt,attk,def,gl,st,fl}

Ityp ityps[ITYP_NUM_ITEMS + MTYP_NUM_MONS] = {
/*  item name              type              weight   A/D     display                      */
	ITYP("long sword",     ITSORT_LONGSWORD,  2000,  10, 0, ITCH_WEAPON | COL_TXT(11,11, 0), 0,
		ITF_EDGE | ITF_POINT),
	ITYP("fencing sword",  ITSORT_LONGSWORD,  1500,  10, 0, ITCH_WEAPON | COL_TXT(11, 0,11), 0,
		ITF_POINT),
	ITYP("axe",            ITSORT_AXE,        3000,  8,  0, ITCH_WEAPON | COL_TXT(11,11, 0), 0,
		ITF_EDGE),
	ITYP("battle-axe",     ITSORT_AXE,        1000,  16, 0, ITCH_WEAPON | COL_TXT(11,11, 0), 0,
		ITF_EDGE),
	ITYP("war hammer",     ITSORT_HAMMER,     5000,  16, 0, ITCH_WEAPON | COL_TXT(15,11, 0), 0,
		0),
	ITYP("dagger",         ITSORT_DAGGER,     100,   6,  0, ITCH_WEAPON | COL_TXT( 0,11, 0), 0,
		ITF_EDGE | ITF_POINT),
	ITYP("short sword",    ITSORT_SHORTSWORD, 2000,  8,  0, ITCH_WEAPON | COL_TXT(11, 8, 0), 0,
		ITF_EDGE | ITF_POINT),
	ITYP("glove",          ITSORT_GLOVE,      70,    0,  1, ITCH_ARMOUR | COL_TXT( 0,11, 0), 0,
		0),
	ITYP("cloth tunic",    ITSORT_TUNIC,      100,   0,  1, ITCH_ARMOUR | COL_TXT(11, 8, 0), 0,
		0),
	ITYP("chain mail",     ITSORT_MAIL,       5000,  0,  8, ITCH_ARMOUR | COL_TXT( 8, 8, 8), 0,
		0),
	ITYP("plate mail",     ITSORT_MAIL,       8000,  0,  9, ITCH_ARMOUR | COL_TXT( 8,11, 0), 0,
		0),
	ITYP("leather hat",    ITSORT_HELM,       50,    0,  1, ITCH_ARMOUR | COL_TXT( 8, 8, 2), 0,
		0),
	ITYP("helmet",         ITSORT_HELM,       2000,  0,  3, ITCH_ARMOUR | COL_TXT(11,11, 2), 0,
		0),
	ITYP("gold piece",     ITSORT_MONEY,      1,     0,  0, ITCH_DOSH   | COL_TXT(15,15, 0), 1,
		0),
	ITYP("bone",           ITSORT_BONE,       100,   0,  0, ITCH_CORPSE | COL_TXT(15,15,15), 1,
		0),
	ITYP("fireball",       ITSORT_ARCANE,     0,     0,  0, 0x09        | COL_TXT(15, 4, 0), 0,
		0),
	ITYP("water bolt",     ITSORT_ARCANE,     0,     0,  0, 0x07        | COL_TXT( 0, 8,15), 0,
		0),
	ITYP("ice bolt",       ITSORT_ARCANE,     0,     0,  0, 0x07        | COL_TXT( 0,15,15), 0,
		0),
	ITYP("force shard",    ITSORT_SHARD,      20,    0,  0, 0xFB        | COL_TXT(15, 8, 0), 0,
		0),
	ITYP("wind shard",     ITSORT_SHARD,      20,    0,  0, 0xFB        | COL_TXT( 0,15,15), 0,
		0),
/*  item name              type              weight   A/D     display                      */
};

char *item_appearance[] = {"MONEY", "WEAPONS", "ARMOUR", "FOOD, DEBRIS", "TOOLS", "", "", "CURIOS", ""};

const int it_displayorder[] = {ITCAT_HANDS, ITCAT_DOSH, ITCAT_WEAPON, ITCAT_ARMOUR, ITCAT_FOOD, ITCAT_JEWEL, -1};

void ityp_init ()
{
	int i;
	for (i = 0; i < MTYP_NUM_MONS; ++ i)
	{
		ityps[ITYP_NUM_ITEMS + i] = (Ityp) ITYP("", ITSORT_CORPSE, CORPSE_WEIGHTS[all_mons[i].mflags>>29], 0, 0, ITCH_CORPSE | (all_mons[i].gl & 0xFFFFFF00), 0, 0);
		snprintf (ityps[ITYP_NUM_ITEMS + i].name, ITEM_NAME_LENGTH, "%s corpse", all_mons[i].mname);
	}
}

void it_ndesc (char *out, int length, const struct Item *item, const struct Monster *pl)
{
	int i = snprintf (out, length, "#g%s a ", gl_format (it_gl (item)));
	if (i >= length)
		return;
	char ench_string[60] = "";
	int a = it_attk (item), d = it_def (item);
	if (a && pl)
		snprintf (ench_string, 60, " (#nF3300000%d+%d#nBBB00000 dmg)",
			a, mons_attk_bonus (pl, item));
	else if (a)
		snprintf (ench_string, 60, " (#nF3300000%d#nBBB00000 dmg)", a);
	else if (d)
		snprintf (ench_string, 60, " (#nF7000000%d#nBBB00000 def)", d);
	snprintf (out + i, length - i, "%s%s%s%s",
			 /* name */
			 it_typename (item),
			 //item->stacksize == 1 ? "" : "s",
			 /* enchantment value */
			 ench_string,
			 /* wielded */
			 it_wieldedID (item) ? " (wielded)" : "",
			 it_worn (item) ? " (worn)" : ""
			 );
	//w_some (temp2, temp, item->stacksize, 128);
}

void it_desc (char *out, const struct Item *item, const struct Monster *pl)
{
	it_ndesc (out, IT_DESC_LEN, item, pl);
}

const char *it_typename (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_typename");
		//return NULL;
	return it_ityp (item)->name;
}

/*int it_can_merge (const struct Item *item1, const struct Item *item2)
{
	return NO_ITEM(item1) || NO_ITEM(item2) ||
		(it_stackable (item1) &&
		 (!memcmp(&item1->atype, &item2->atype, sizeof(Ityp))) &&
		 item1->name == item2->name &&
		 item1->attr == item2->attr &&
		 item1->attk == item2->attk &&
		 item1->def  == item2->def);
}*/

/*int it_weight (const struct Item *item)
{
	return item->cur_weight;
}*/

/*int it_merge (struct Item *it1, struct Item *it2)
{
	if (!it_can_merge (it1, it2))
		return 0;
	it1->stacksize += it2->stacksize;
	it1->cur_weight += it2->cur_weight;
	return 1;
}*/

void item_gen (union ItemLoc loc)
{
	struct Item item;
	enum ITEM_TYPE typ = rn(14);
	/*if (typ == ITYP_GOLD_PIECE)
	{
		int stacksize = rn(50)+30;
		item = new_items (ityps[typ], stacksize);
	}
	else*/
		item = new_item (typ);
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

int it_persistent (const struct Item *item)
{
	return it_sort (item) != ITSORT_ARCANE;
}

int it_canwear (const struct Item *item, enum MONS_BODYPART part)
{
	switch (part)
	{
		case MONS_HAND:
			return it_sort (item) == ITSORT_GLOVE;
		case MONS_TORSO:
			return it_sort (item) == ITSORT_TUNIC ||
			       it_sort (item) == ITSORT_MAIL;
		case MONS_HEAD:
			return it_sort (item) == ITSORT_HELM;
		case MONS_FOOT:
			return 0;
	}
	return 0;
}

void it_wear (struct Item *item, size_t offset)
{
	if (!item)
		panic ("NULL item in it_wear");
	item->worn_offset = offset;
}

void it_unwear (struct Item *item)
{
	if (!item)
		panic ("NULL item in it_unwear");
	item->worn_offset = -1;
}

int it_freeze (struct Item *item)
{
	if (it_sort (item) != ITSORT_ARCANE)
		return 1;
	if (it_type (item) == ITYP_FIREBALL)
	{
		item_free (item);
		p_msg ("The fire goes out!");
		return 0;
	}
	if (it_type (item) == ITYP_WATER_BOLT)
	{
		it_type (item) = ITYP_ICE_BOLT;
		p_msg ("The water freezes into an ice bolt!");
		return 1;
	}
	return 1;
}

int it_burn (struct Item *it)
{
	if (it_sort (it) != ITSORT_SHARD)
		return 1;

	it_break (it);
	return 1;
}

int it_projdamage (const struct Item *item)
{
	if (it_category (it_sort (item)) == ITCAT_WEAPON)
		return rn(it_attk (item) + 1)/2;
	return rn(3);
}

int it_attk (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_attk");
	return item->attk;
}

void it_set_attk (struct Item *item, int attk)
{
	if (!item)
		panic ("NULL item in it_set_attk");
	item->attk = attk;
}

int it_def (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_def");
	return item->def;
}

int it_worn_offset (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_worn_offset");
	return item->worn_offset;
}

int it_worn (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_worn");
	return item->worn_offset != -1;
}

int it_weight (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_weight");
	return item->wt;
}

glyph it_gl (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_gl");
	return it_ityp (item)->gl;
}

enum ITSORT it_sort (const struct Item *item)
{
	return ityps [it_type (item)].sort;
}

void it_rem (struct Item *item)
{
	if (!item)
		panic ("NULL item in it_gl");
	item->ID = 0;
}

int it_fragile (const struct Item *item)
{
	if (!item)
		panic ("NULL item in it_fragile");
	return it_sort (item) == ITSORT_SHARD;
}

void it_break (struct Item *item)
{
	if (!item)
		panic ("NULL item in it_break");
	if (it_type (item) == ITYP_FORCE_SHARD)
	{
		ev_queue (0, (union Event) { .item_explode = {EV_ITEM_EXPLODE, item->ID, 5}});
		return;
	}
	item_free (item);
}

enum DMG_TYPE it_dtyp (const struct Item *item)
{
	if (it_ityp (item)->flags & (ITF_EDGE | ITF_POINT))
		return DTYP_CUT;
	return DTYP_BLUNT;
}

enum SK_TYPE it_skill (const struct Item *item)
{
	if (NO_ITEM(item))
		return SK_USE_MARTIAL_ARTS;
	switch (it_sort (item))
	{
		case ITSORT_LONGSWORD:
			return SK_USE_LONGSWORD;
		case ITSORT_AXE:
			return SK_USE_AXE;
		case ITSORT_HAMMER:
			return SK_USE_HAMMER;
		case ITSORT_DAGGER:
			return SK_USE_DAGGER;
		case ITSORT_SHORTSWORD:
			return SK_USE_SHORTSWORD;
		default:
			return SK_NONE;
	}
}

enum ITCAT it_category (enum ITSORT type)
{
	switch (type)
	{
	case ITSORT_NONE:
		break;
	case ITSORT_LONGSWORD:
	case ITSORT_AXE:
	case ITSORT_HAMMER:
	case ITSORT_DAGGER:
	case ITSORT_SHORTSWORD:
		return ITCAT_WEAPON;
	case ITSORT_CHEST:
		return ITCAT_TOOL;
	case ITSORT_GLOVE:
	case ITSORT_TUNIC:
	case ITSORT_MAIL:
	case ITSORT_HELM:
		return ITCAT_ARMOUR;
	case ITSORT_CORPSE:
	case ITSORT_BONE:
		return ITCAT_FOOD;
	case ITSORT_MONEY:
		return ITCAT_DOSH;
	case ITSORT_ARCANE:
		return ITCAT_STRANGE;
	case ITSORT_SHARD:
		return ITCAT_JEWEL;
	}
	panic ("end reached in it_category");
	return -1;
}

MID it_wieldedID (const struct Item *item)
{
	if (!item)
		return 0;
	if (item->loc.loc == LOC_WIELDED)
		return item->loc.wield.monsID;
	return 0;
}

MID it_invID (const struct Item *item)
{
	if (!item)
		return 0;
	if (item->loc.loc == LOC_INV)
		return item->loc.inv.monsID;
	return 0;
}

int it_dlvl (const struct Item *item)
{
	return item->loc.loc == LOC_DLVL;
}

int it_flight (const struct Item *item)
{
	return item->loc.loc == LOC_FLIGHT;
}

