#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "include/all.h"
#include "include/drawing.h"

enum MONS_BODYPART;
enum SK_TYPE;

enum ITCAT
{
	ITCAT_HANDS = 0,
	ITCAT_DOSH,
	ITCAT_WEAPON,
	ITCAT_ARMOUR,
	ITCAT_FOOD,
	ITCAT_TOOL,
	ITCAT_STRANGE,
	//ITCAT_JEWEL,
	ITCAT_END
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
	ITSORT_SHARD,
	ITSORT_TURRET
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
	ITYP_FIRE_TURRET,
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
	LOC_WIELDED//,  /* wielded by a monster */
	//LOC_FLIGHT    /* in flight in the dungeon */
};

struct ItemInDlvl
{
	enum ITEM_LOC loc;
	int dlevel;
	int zloc, yloc, xloc;
};

struct ItemInInv
{
	enum ITEM_LOC loc;
	MonsID monsID;
	int invnum;
};

struct ItemWielded
{
	enum ITEM_LOC loc;
	MonsID monsID;
	int invnum;
	int arm;
};

#define switch_loc(item) \
struct ItemInDlvl dlvl; struct ItemInInv inv; struct ItemWielded wield; \
switch (it_get_loc ((item), &dlvl, &inv, &wield))

union ItemLoc
{
	enum ITEM_LOC loc;
	struct ItemInDlvl dlvl;
	struct ItemInInv inv;
	struct ItemWielded wield;
};

struct ItemStatus
{
#include "auto/item.status.h"
};

/* an actual physical item */
struct Item_internal
{
	ItemID ID;
	union ItemLoc loc;
	struct ItemStatus status;
	enum ITEM_TYPE qtype;
	//uint32_t attr;
	int wt;
	int attk, def;
	int worn_offset;
};

// TODO use!
/*struct ItemStack
{
	ItemID ID;
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
};*/

extern Ityp ityps[];

#define new_item(typ) ((struct Item_internal) {0, { .loc = LOC_NONE}, {{0,},}, (typ), ityps[typ].wt, ityps[typ].attk, ityps[typ].def, -1})

void ityp_init      ();

void item_piles     (int, Vector, Vector);
void item_gen       (union ItemLoc);

extern char *item_appearance[];

/* Item helper functions with no side-effects: */

/* are two items equal? */
int  items_equal (ItemID , ItemID );

/* get near and far descriptions */
void it_ndesc (char *out, int length, ItemID item, MonsID player);
void it_desc (char *out, ItemID item, MonsID player); /* using default max output length */

/* get item properties */
int it_is (ItemID item);
struct Item_internal *it_internal (ItemID item);

enum ITEM_TYPE it_type (ItemID item);
const char *it_typename (ItemID item);
int it_attk (ItemID item);
int it_def (ItemID item);
glyph it_gl (ItemID item);
int it_stackable (ItemID item);
int it_fragile (ItemID item); /* will break when thrown */
int it_persistent (ItemID item); /* can exist stably */
int it_projdamage (ItemID item);
int it_weight (ItemID item);
int it_base_weight (ItemID item);
int it_worn_offset (ItemID item);
int it_worn (ItemID item);
enum DMG_TYPE it_dtyp (ItemID item);

/* get mergibility of two item stacks */
//int it_can_merge (const ItemID it1, const ItemID it2);

/* is undergoing event */
#define it_event(it, ev) (it_internal (it)->status.ev.evID)

int it_flag (ItemID, int);

enum ITEM_TYPE it_type (ItemID);

/* can wear in a particular place */
int it_canwear (ItemID item, enum MONS_BODYPART part);

/* get item ITSORT */
enum ITSORT it_sort (ItemID item);

/* get skill used by item */
enum SK_TYPE it_skill (ItemID item);

/* get associated item category */
enum ITCAT it_category (enum ITSORT type);

/* get item location */
enum ITEM_LOC it_loc (ItemID item);
enum ITEM_LOC it_get_loc (ItemID item,
	struct ItemInDlvl *, struct ItemInInv *, struct ItemWielded * /*, struct ItemInFlight * */);
int it_dlvl (ItemID item, struct ItemInDlvl *);
int it_inv (ItemID item, struct ItemInInv *);
int it_wield (ItemID item, struct ItemWielded *);

/* Side-effects: */

/* freeze an item; return whether item still exists */
int it_freeze (ItemID item);

/* burn an item; return whether item still exists */
int it_burn (ItemID item);

/* merge second stack to first stack if possible */
//int it_merge (ItemID it1, ItemID it2);

/* break an item */
void it_break (ItemID item);

/* change item worn property */
void it_wear (ItemID item, size_t offset);
void it_unwear (ItemID item);

/* set item attr value */
void it_set_attk (ItemID item, int attk);
void it_set_def (ItemID item, int def);

/* put an item on the floor */
void it_fl_to_dlv (ItemID item);

/* fill the location for a flying object starting from a monster */
//union ItemLoc it_monsfloc (MonsID mons, int speed);

/* return loc for monster in dgn */
union ItemLoc it_monsdloc (MonsID mons);

/* shoot an item */
void it_shoot (ItemID item, int zloc, int yloc, int xloc);

#endif /* ITEM_H_INCLUDED */

