/* item.c */

#include "include/thing.h"
#include "include/words.h"
#include "include/world.h"
#include "include/monst.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/item.h"
#include "include/dlevel.h"
#include "include/rand.h"
#include "include/skills.h"
#include "include/event.h"
#include "include/debug.h"

#include <stdio.h>
#include <string.h>

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
	ITYP("battle-axe",     ITSORT_AXE,        5000,  16, 0, ITCH_WEAPON | COL_TXT(11,11, 0), 0,
		ITF_EDGE),
	ITYP("fire axe",       ITSORT_AXE,        3000,  16, 0, ITCH_WEAPON | COL_TXT(15,11, 0), 0,
		ITF_EDGE | ITF_FIRE_EFFECT),
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
	ITYP("fire shard",     ITSORT_SHARD,      20,    0,  0, 0xFB        | COL_TXT(15, 8, 0), 0,
		0),
	ITYP("wind shard",     ITSORT_SHARD,      20,    0,  0, 0xFB        | COL_TXT( 0,15,15), 0,
		0),
	ITYP("fire turret",    ITSORT_TURRET,     20,    0,  0, 0xE2        | COL_TXT(15, 2, 0), 0,
		0),
/*  item name              type              weight   A/D     display                      */
};

char *item_appearance[] = {"(HANDS)", "Money", "Weapons", "Armour", "Food, debris", "Tools", "Curios", ""};

void ityp_init ()
{
	int i;
	for (i = 0; i < MTYP_NUM_MONS; ++ i)
	{
		ityps[ITYP_NUM_ITEMS + i] = (Ityp) ITYP("", ITSORT_CORPSE, CORPSE_WEIGHTS[mons_types[i].mflags>>29], 0, 0, ITCH_CORPSE | (mons_types[i].gl & 0xFFFFFF00), 0, 0);
		snprintf (ityps[ITYP_NUM_ITEMS + i].name, ITEM_NAME_LENGTH, "%s corpse", mons_types[i].mname);
	}
}

int it_is (ItemID item)
{
	return world.items->data[item].ID == item;
}

struct Item_internal *it_internal (ItemID item)
{
	return &world.items->data[item];
}

enum ITEM_TYPE it_type (ItemID item)
{
	return it_internal (item)->qtype;
}

int it_flag (ItemID item, int f)
{
	return ityps[it_type (item)].flags&f;
}

void it_ndesc (char *out, int length, ItemID item, MonsID pl)
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
			 it_wield (item, NULL) ? " (wielded)" : "",
			 it_worn (item) ? " (worn)" : ""
			 );
	//w_some (temp2, temp, item->stacksize, 128);
}

void it_desc (char *out, ItemID item, MonsID pl)
{
	it_ndesc (out, IT_DESC_LEN, item, pl);
}

const char *it_typename (ItemID item)
{
	return it_ityp (item)->name;
}

/*int it_can_merge (ItemID item1, ItemID item2)
{
	return it_no(item1) || it_no(item2) ||
		(it_stackable (item1) &&
		 (!memcmp(&item1->atype, &item2->atype, sizeof(Ityp))) &&
		 item1->name == item2->name &&
		 item1->attr == item2->attr &&
		 item1->attk == item2->attk &&
		 item1->def  == item2->def);
}*/

/*int it_weight (ItemID item)
{
	return item->cur_weight;
}*/

/*int it_merge (ItemID it1, ItemID it2)
{
	if (!it_can_merge (it1, it2))
		return 0;
	it1->stacksize += it2->stacksize;
	it1->cur_weight += it2->cur_weight;
	return 1;
}*/

void item_gen (union ItemLoc loc)
{
	struct Item_internal item;
	enum ITEM_TYPE typ = rn(14);
	/*if (typ == ITYP_GOLD_PIECE)
	{
		int stacksize = rn(50)+30;
		item = new_items (ityps[typ], stacksize);
	}
	else*/
		item = new_item (typ);
	it_create (&item, loc);
}

int it_persistent (ItemID item)
{
	return it_sort (item) != ITSORT_ARCANE;
}

int it_canwear (ItemID item, enum MONS_BODYPART part)
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

void it_wear (ItemID item, size_t offset)
{
	if (!item)
		panic ("invalid item in it_wear");
	struct Item_internal *ii = it_internal (item);
	ii->worn_offset = offset;
}

void it_unwear (ItemID item)
{
	if (!item)
		panic ("invalid item in it_unwear");
	struct Item_internal *ii = it_internal (item);
	ii->worn_offset = -1;
}

int it_freeze (ItemID item)
{
	if (it_sort (item) != ITSORT_ARCANE)
		return 1;
	if (it_type (item) == ITYP_FIREBALL)
	{
		it_destroy (item);
		p_msg ("The fire goes out!");
		return 0;
	}
	if (it_type (item) == ITYP_WATER_BOLT)
	{
		it_internal (item)->qtype = ITYP_ICE_BOLT;
		p_msg ("The water freezes into an ice bolt!");
		return 1;
	}
	return 1;
}

int it_burn (ItemID it)
{
	if (it_sort (it) != ITSORT_SHARD)
		return 1;

	it_break (it);
	return 1;
}

int it_projdamage (ItemID item)
{
	if (it_category (it_sort (item)) == ITCAT_WEAPON)
		return rn(it_attk (item) + 1)/2;
	return rn(3);
}

int it_attk (ItemID item)
{
	if (!item)
		panic ("NULL item in it_attk");
	struct Item_internal *ii = it_internal (item);
	return ii->attk;
}

void it_set_attk (ItemID item, int attk)
{
	if (!item)
		panic ("NULL item in it_set_attk");
	struct Item_internal *ii = it_internal (item);
	ii->attk = attk;
}

int it_def (ItemID item)
{
	if (!item)
		panic ("NULL item in it_def");
	struct Item_internal *ii = it_internal (item);
	return ii->def;
}

int it_worn_offset (ItemID item)
{
	if (!item)
		panic ("NULL item in it_worn_offset");
	struct Item_internal *ii = it_internal (item);
	return ii->worn_offset;
}

int it_worn (ItemID item)
{
	if (!item)
		panic ("NULL item in it_worn");
	struct Item_internal *ii = it_internal (item);
	return ii->worn_offset != -1;
}

int it_weight (ItemID item)
{
	if (!item)
		panic ("NULL item in it_weight");
	struct Item_internal *ii = it_internal (item);
	return ii->wt;
}

glyph it_gl (ItemID item)
{
	if (!item)
		panic ("NULL item in it_gl");
	return it_ityp (item)->gl;
}

enum ITSORT it_sort (ItemID item)
{
	return ityps [it_type (item)].sort;
}

int it_fragile (ItemID item)
{
	if (!item)
		panic ("NULL item in it_fragile");
	return it_sort (item) == ITSORT_SHARD;
}

void it_break (ItemID item)
{
	if (!item)
		panic ("NULL item in it_break");
	if (it_type (item) == ITYP_FORCE_SHARD)
	{
		ev_queue (0, item_explode, item, rn(5)+5);
		return;
	}
	it_destroy (item);
}

enum DMG_TYPE it_dtyp (ItemID item)
{
	if (!item)
		return DTYP_BLUNT;
	if (it_ityp (item)->flags & (ITF_EDGE | ITF_POINT))
		return DTYP_CUT;
	return DTYP_BLUNT;
}

enum SK_TYPE it_skill (ItemID item)
{
	if (!item)
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
	case ITSORT_SHARD:
	case ITSORT_TURRET:
		return ITCAT_STRANGE;
	//	return ITCAT_JEWEL;
	//	return ITCAT_STRANGE;
	}
	panic ("end reached in it_category");
	return -1;
}

int it_wield (ItemID item, struct ItemWielded *w)
{
	return it_get_loc (item, NULL, NULL, w) == LOC_WIELDED;
}

int it_inv (ItemID item, struct ItemInInv *i)
{
	return it_get_loc (item, NULL, i, NULL) == LOC_INV;
}

int it_dlvl (ItemID item, struct ItemInDlvl *d)
{
	return it_get_loc (item, d, NULL, NULL) == LOC_DLVL;
}

enum ITEM_LOC it_get_loc (ItemID item, struct ItemInDlvl *d, struct ItemInInv *i, struct ItemWielded *w)
{
	struct Item_internal *ii = it_internal (item);
	switch (ii->loc.loc)
	{
	case LOC_NONE:
		break;
	case LOC_DLVL:
		if (d)
			*d = ii->loc.dlvl;
		break;
	case LOC_INV:
		if (i)
			*i = ii->loc.inv;
		break;
	case LOC_WIELDED:
		if (w)
			*w = ii->loc.wield;
		break;
	}
	return ii->loc.loc;
}

enum ITEM_LOC it_loc (ItemID item)
{
	return it_internal (item)->loc.loc;
}

union ItemLoc it_monsdloc (MonsID mons)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	return (union ItemLoc) {.dlvl = {LOC_DLVL, d, z, y, x}};
}

void it_shoot (ItemID item, int zdest, int ydest, int xdest)
{
	struct ItemInDlvl d;
	if (!it_dlvl (item, &d))
		return;
	if (ydest == d.yloc && xdest == d.xloc)
		return;
	struct Item_internal newitem = new_item (ITYP_FIREBALL);
	union ItemLoc loc = { .dlvl = d};
	struct BresState bres;
	bres_init (&bres, d.yloc, d.xloc, ydest, xdest);

	ItemID bullet = it_create (&newitem, loc);
	it_set_attk (bullet, 2);
	int speed = 100;
	ev_queue (60, proj_move, bullet, bres, speed, 0);
}

