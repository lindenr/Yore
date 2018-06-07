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
	ITYP("cloth tunic",    ITSORT_TUNIC,      100,   0,  1,      ITCH_ARMOUR | COL_TXT(11, 8, 0), 0),
	ITYP("chain mail",     ITSORT_MAIL,       5000,  0,  8,      ITCH_ARMOUR | COL_TXT( 8, 8, 8), 0),
	ITYP("plate mail",     ITSORT_MAIL,       8000,  0,  9,      ITCH_ARMOUR | COL_TXT( 8,11, 0), 0),
	ITYP("leather hat",    ITSORT_HELM,       50,    0,  1,      ITCH_ARMOUR | COL_TXT( 8, 8, 2), 0),
	ITYP("helmet",         ITSORT_HELM,       2000,  0,  3,      ITCH_ARMOUR | COL_TXT(11,11, 2), 0),
	ITYP("gold piece",     ITSORT_MONEY,      1,     0,  0,      ITCH_DOSH   | COL_TXT(15,15, 0), 1),
	ITYP("bone",           ITSORT_BONE,       100,   0,  0,      ITCH_CORPSE | COL_TXT(15,15,15), 1),
	ITYP("fireball",       ITSORT_ARCANE,     0,     0,  0,      0x09        | COL_TXT(15, 4, 0), 0),
	ITYP("water bolt",     ITSORT_ARCANE,     0,     0,  0,      0x07        | COL_TXT( 0, 8,15), 0),
	ITYP("ice bolt",       ITSORT_ARCANE,     0,     0,  0,      0x07        | COL_TXT( 0,15,15), 0),
	ITYP("force shard",    ITSORT_SHARD,      20,    0,  0,      0xFB        | COL_TXT(15, 8, 0), 0),
	ITYP("wind shard",     ITSORT_SHARD,      20,    0,  0,      0xFB        | COL_TXT( 0,15,15), 0),
	ITYP("",               ITSORT_NONE,       0,     0,  0,      0                              , 0)
/*  item name              type              weight attributes     display                      */
};

char *item_appearance[] = {"MONEY", "WEAPONS", "ARMOUR", "FOOD, DEBRIS", "TOOLS", "", "", "CURIOS"};

const int it_displayorder[] = {ITCAT_DOSH, ITCAT_WEAPON, ITCAT_ARMOUR, ITCAT_FOOD, ITCAT_JEWEL, -1};

void ityp_init ()
{
	return;
}

char *it_desc (const struct Item *item, const struct Monster *pl)
{
	char *ret = malloc(128);
	char temp2[120];
	char temp[115];
	if (item->name != NULL && item->name[0] != '\0')
	{
		p_msg ("NULULULULUL");
	}
	else
	{
		char ench_string[50] = "";
		if (item->attk && pl)
			snprintf (ench_string, 50, " (#nF3300000%d+%d#nBBB00000 dmg)",
				item->attk, mons_attk_bonus (pl, item));
		else if (item->attk)
			snprintf (ench_string, 50, " (#nF3300000%d#nBBB00000 dmg)",
				item->attk);
		else if (item->def)
			snprintf (ench_string, 50, " (#nF7000000%d#nBBB00000 def)", item->def);
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
		         it_wieldedID (item) ? " (wielded)" : "",
				 item_worn (item) ? " (worn)" : ""
				 );
		w_some (temp2, temp, item->stacksize, 128);
		snprintf (ret, 128, "#g%s %s", gl_format (item->type.gl), temp2);
	}
	return ret;
}

int it_can_merge (const struct Item *item1, const struct Item *item2)
{
	return NO_ITEM(item1) || NO_ITEM(item2) ||
		(item1->type.stackable &&
		 (!memcmp(&item1->type, &item2->type, sizeof(Ityp))) &&
		 item1->name == item2->name &&
		 item1->attr == item2->attr &&
		 item1->attk == item2->attk &&
		 item1->def  == item2->def);
}

int it_weight (const struct Item *item)
{
	return item->cur_weight;
}

int it_merge (struct Item *it1, struct Item *it2)
{
	if (!it_can_merge (it1, it2))
		return 0;
	it1->stacksize += it2->stacksize;
	it1->cur_weight += it2->cur_weight;
	return 1;
}

void item_gen (union ItemLoc loc)
{
	struct Item item;
	enum ITEM_TYPE typ = rn(14);
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

int it_persistent (const struct Item *item)
{
	return item->type.type != ITSORT_ARCANE;
}

int it_canwear (const struct Item *item, enum MONS_BODYPART part)
{
	switch (part)
	{
		case MONS_HAND:
			return item->type.type == ITSORT_GLOVE;
		case MONS_TORSO:
			return item->type.type == ITSORT_TUNIC ||
			       item->type.type == ITSORT_MAIL;
		case MONS_HEAD:
			return item->type.type == ITSORT_HELM;
		case MONS_FOOT:
			return 0;
	}
	return 0;
}

int it_freeze (struct Item *it)
{
	if (it->type.type != ITSORT_ARCANE)
		return 1;
	if (!strcmp (it->type.name, "fireball"))
	{
		item_free (it);
		p_msg ("The fire goes out!");
		return 0;
	}
	if (!strcmp (it->type.name, "water bolt"))
	{
		memcpy (&it->type, &ityps[ITYP_ICE_BOLT], sizeof(Ityp));
		p_msg ("The water freezes into an ice bolt!");
		return 1;
	}
	return 1;
}

int it_projdamage (const struct Item *item)
{
	if (it_category (item->type.type) == ITCAT_WEAPON)
		return rn(item->attk + 1)/2;
	return rn(3);
}

enum SK_TYPE it_skill (const struct Item *item)
{
	if (NO_ITEM(item))
		return SK_USE_MARTIAL_ARTS;
	switch (item->type.type)
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
	if (item->loc.loc == LOC_WIELDED)
		return item->loc.wield.monsID;
	return 0;
}

MID it_invID (const struct Item *item)
{
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

