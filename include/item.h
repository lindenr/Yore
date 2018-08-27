#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "include/drawing.h"

typedef int TID, MID;
struct Pack;
struct Monster;
enum MONS_BODYPART;
enum SK_TYPE;

enum ITCAT
{
	ITCAT_DOSH = 0,
	ITCAT_WEAPON,
	ITCAT_ARMOUR,
	ITCAT_FOOD,
	ITCAT_TOOL,
	ITCAT_STRANGE,
	ITCAT_CHARM,
	ITCAT_JEWEL,
	ITCAT_HANDS
};

enum ITSORT
{
	ITSORT_NONE = 0,  /* placeholder - always useful */
	ITSORT_LONGSWORD,
	ITSORT_AXE,
	ITSORT_HAMMER,
	ITSORT_DAGGER,
	ITSORT_SHORTSWORD,
	ITSORT_CHEST,
	ITSORT_GLOVE,
	ITSORT_TUNIC,
	ITSORT_MAIL,
	ITSORT_HELM,
	ITSORT_CORPSE,
	ITSORT_BONE,
	ITSORT_MONEY,
	ITSORT_ARCANE,
	ITSORT_SHARD
};

extern const int it_displayorder[];

enum ITEM_TYPE
{
	ITYP_NONE = -1,
	ITYP_LONG_SWORD = 0,
	ITYP_FENCING_SWORD,
	ITYP_AXE,
	ITYP_BATTLE_AXE,
	ITYP_FIRE_AXE,
	ITYP_WAR_HAMMER,
	ITYP_DAGGER,
	ITYP_SHORT_SWORD,
	ITYP_GLOVE,
	ITYP_CLOTH_TUNIC,
	ITYP_CHAIN_MAIL,
	ITYP_PLATE_MAIL,
	ITYP_LEATHER_HAT,
	ITYP_HELMET,
	ITYP_GOLD_PIECE,
	ITYP_BONE,
	ITYP_FIREBALL,
	ITYP_WATER_BOLT,
	ITYP_ICE_BOLT,
	ITYP_FORCE_SHARD,
	ITYP_WIND_SHARD,
	ITYP_NUM_ITEMS
	/* corpses here at runtime */
};

#define ITCH_WEAPON  ')'
#define ITCH_TOOL    '('
#define ITCH_STRANGE ']'
#define ITCH_ARMOUR  '['
#define ITCH_FOOD    '%'
#define ITCH_CORPSE  ITCH_FOOD
#define ITCH_DOSH    '$'
#define ITCH_CHARM   '='
#define ITCH_JEWEL    7

#define ITCAT_ALL     0xFFFFFEFF // misses out hands

#define ITEM_NAME_LENGTH 20
#define it_no(item) ((!(item)) || ((item)->ID == 0))
#define it_type(item) ((item)->qtype)
#define it_ityp(item) (&ityps[it_type(item)])

#define ITF_EDGE 0x1
#define ITF_POINT 0x2
#define ITF_FIRE_EFFECT 0x4

/* type of item */
typedef struct
{
	char name[ITEM_NAME_LENGTH];/* name of that type of item */
	enum ITSORT sort;           /* sort */
	uint32_t wt;				/* weight */
	int attk, def;              /* attack, defense stats */
	glyph gl;					/* for the display */
	int stackable;              /* does it stack */
	uint32_t flags;             /* flags */
} Ityp;

enum ITEM_LOC
{
	LOC_NONE = 0, /* placeholder */
	LOC_DLVL,     /* on the ground */
	LOC_INV,      /* in a monster inventory (or chest?) */
	LOC_WIELDED,  /* wielded by a monster */
	LOC_FLIGHT    /* in flight in the dungeon */
};

struct ItemInDlvl
{
	enum ITEM_LOC loc;
	int dlevel;
	uint32_t yloc, xloc;
};

struct ItemInInv
{
	enum ITEM_LOC loc;
	TID monsID;
	int invnum;
};

struct ItemWielded
{
	enum ITEM_LOC loc;
	TID monsID;
	int invnum;
	int arm;
};

struct ItemInFlight
{
	enum ITEM_LOC loc;
	int dlevel;
	uint32_t yloc, xloc;
	struct BresState bres;
	int speed;
	TID frID;
};

union ItemLoc
{
	enum ITEM_LOC loc;
	struct ItemInDlvl dlvl;
	struct ItemInInv inv;
	struct ItemWielded wield;
	struct ItemInFlight fl;
};

/* an actual physical item */
struct Item
{
	TID ID;
	union ItemLoc loc;
	enum ITEM_TYPE qtype;
	//uint32_t attr;
	int wt;
	int attk, def;
	int worn_offset;
};

// TODO use!
struct ItemStack
{
	TID ID;
	union ItemLoc loc;
	int stacksize;
	int custom;
	union
	{
		Ityp *ptype;
		Ityp type;
	} type;
	uint32_t attr;
	Vector data;
};

extern Ityp ityps[];

#define new_item(typ) ((struct Item) {0, { .loc = LOC_NONE}, (typ), ityps[typ].wt, ityps[typ].attk, ityps[typ].def, -1})

extern struct Item no_item;

void ityp_init      ();

void item_piles     (int, Vector, Vector);
void item_gen       (union ItemLoc);

extern char *item_appearance[];

/* Item helper functions with no side-effects: */

/* are two items equal? */
int  items_equal (struct Item *, struct Item *);

/* get near and far descriptions */
void it_ndesc (char *out, int length, const struct Item *item, const struct Monster *player);
void it_desc (char *out, const struct Item *item, const struct Monster *player); /* using default max output length */

/* get item properties */
const char *it_typename (const struct Item *item);
int it_attk (const struct Item *item);
int it_def (const struct Item *item);
glyph it_gl (const struct Item *item);
int it_stackable (const struct Item *item);
int it_fragile (const struct Item *item); /* will break when thrown */
int it_persistent (const struct Item *item); /* can exist stably */
int it_projdamage (const struct Item *item);
int it_weight (const struct Item *item);
int it_base_weight (const struct Item *item);
int it_worn_offset (const struct Item *item);
int it_worn (const struct Item *item);
enum DMG_TYPE it_dtyp (const struct Item *item);

/* get mergibility of two item stacks */
//int it_can_merge (const struct Item *it1, const struct Item *it2);

/* can wear in a particular place */
int it_canwear (const struct Item *item, enum MONS_BODYPART part);

/* get item ITSORT */
enum ITSORT it_sort (const struct Item *item);

/* get skill used by item */
enum SK_TYPE it_skill (const struct Item *item);

/* get associated item category */
enum ITCAT it_category (enum ITSORT type);

/* get item location */
MID it_wieldedID (const struct Item *item);
MID it_invID (const struct Item *item);
int it_dlvl (const struct Item *item);
int it_flight (const struct Item *item);

/* Side-effects: */

/* freeze an item; return whether item still exists */
int it_freeze (struct Item *item);

/* burn an item; return whether item still exists */
int it_burn (struct Item *item);

/* merge second stack to first stack if possible */
//int it_merge (struct Item *it1, struct Item *it2);

/* invalidate an item, but do not free it */
void it_rem (struct Item *item);

/* break an item */
void it_break (struct Item *item);

/* change item worn property */
void it_wear (struct Item *item, size_t offset);
void it_unwear (struct Item *item);

/* set item attr value */
void it_set_attk (struct Item *item, int attk);
void it_set_def (struct Item *item, int def);

#endif /* ITEM_H_INCLUDED */

