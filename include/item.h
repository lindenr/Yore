#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "include/drawing.h"

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
	ITSORT_MAIL,
	ITSORT_HELM,
	ITSORT_CORPSE,
	ITSORT_MONEY,
	ITSORT_ARCANE
};

enum ITEM_TYPE
{
	ITYP_NONE = -1,
	ITYP_LONG_SWORD = 0,
	ITYP_FENCING_SWORD,
	ITYP_AXE,
	ITYP_BATTLE_AXE,
	ITYP_WAR_HAMMER,
	ITYP_DAGGER,
	ITYP_SHORT_SWORD,
	ITYP_GLOVE,
	ITYP_CHAIN_MAIL,
	ITYP_PLATE_MAIL,
	ITYP_HELMET,
	ITYP_GOLD_PIECE,
	ITYP_FIREBALL,
	ITYP_WATER_BOLT,
	ITYP_ICE_BOLT,
	NUM_ITYPS
};

/* BUC status */
#define ITEM_CURS   0x00000001
#define ITEM_UNCS   0x00000000
#define ITEM_BLES   0x00000002
#define ITEM_KBUC   0x00000004
/* 1 <= n < 4 */
#define ITEM_PLUS(n) 0x00000000+(n<<3)
#define ITEM_MINS(n) 0x00000020+(n<<3)
/* greased */
#define ITEM_GREASED 0x00000040

#define ITCH_WEAPON  ')'
#define ITCH_TOOL    '('
#define ITCH_STRANGE ']'
#define ITCH_ARMOUR  '['
#define ITCH_FOOD    '%'
#define ITCH_CORPSE  ITCH_FOOD
#define ITCH_DOSH    '$'
#define ITCH_CORPSE  ITCH_FOOD
#define ITCH_CHARM   '='
#define ITCH_JEWEL    7

#define ITCAT_DOSH    0x0001
#define ITCAT_WEAPON  0x0002
#define ITCAT_ARMOUR  0x0004
#define ITCAT_FOOD    0x0008
#define ITCAT_TOOL    0x0010
#define ITCAT_STRANGE 0x0020
#define ITCAT_CHARM   0x0040
#define ITCAT_JEWEL   0x0080

#define ITCAT_ALL     0xFFFF

#define ITEM_NAME_LENGTH 20
#define NO_ITEM(item) ((!(item)) || ((item)->type.type == ITSORT_NONE))

typedef int TID;
struct Pack;
struct Monster;

/* type of item */
typedef struct
{
	char name[ITEM_NAME_LENGTH];/* name of that type of item */
	enum ITSORT type;           /* type */
	uint32_t wt;				/* weight */
	int attk, def;              /* attack, defense stats */
	glyph gl;					/* for the display */
	int stackable;              /* does it stack */
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
	Ityp type;
	uint32_t cur_weight;
	char *name;
	uint32_t attr;
	int attk, def;
	int worn_offset;
	int stacksize;
};

extern Ityp ityps[];

#define new_item(ityp) ((struct Item)\
	{0, { .loc = LOC_NONE}, (ityp), (ityp).wt, NULL, 0, (ityp).attk, (ityp).def, -1, 1})

extern struct Item no_item;

void ityp_init      ();

void item_piles     (int, Vector, Vector);
void item_gen       (union ItemLoc);

#define item_worn(item) ((item)->worn_offset != -1)

char *get_near_desc (const struct Monster *mons, const struct Item *item);
char *get_item_desc (const struct Item);
void item_look      (const struct Item *);
char *get_inv_line  (const struct Item *);
int  items_equal    (struct Item *, struct Item *);

int it_can_merge    (struct Monster *, struct Item *, struct Item *);

#endif /* ITEM_H_INCLUDED */

