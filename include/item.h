#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "include/thing.h"

enum ITYP
{
	ITYP_NONE = 0,				/* placeholder - always useful */
	ITYP_LONGSWORD,
	ITYP_AXE,
	ITYP_HAMMER,
	ITYP_DAGGER,
	ITYP_SHORTSWORD,
	ITYP_CHEST,
	ITYP_GLOVES,
	ITYP_CORPSE,
	ITYP_EGG,
	ITYP_MONEY,
	ITYP_CHARM,
	ITYP_JEWEL
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
/* being worn or applied */
#define ITEM_USED    0x00000080
/* wielded */
#define ITEM_WIELDED 0x00000100

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

#define ITCAT_ALL     (-1)

#define ITEM_INIT = {0,0,""}
#define ITEM_NAME_LENGTH 20

/* type of item */
typedef struct
{
	char name[ITEM_NAME_LENGTH];/* name of that type of item */
	enum ITYP type;             /* type */
	uint32_t wt;				/* weight */
	uint32_t attr;				/* can be used for damage (weapons) (8 bits) */
	glyph gl;					/* for the display */
} Ityp;

/* an actual physical item */
struct Item
{
	Ityp type;
	uint32_t attr;
	uint32_t cur_weight;
	char *name;
};

struct Pack;

extern Ityp items[];
extern int NUM_ITEMS;

void item_piles     (int, Vector, Vector);
void ask_items      (Vector, Vector, const char *);

char *get_item_desc (const struct Item);
void item_look      (const struct Item *);
char *get_inv_line  (struct Pack *, const struct Item *);

#endif /* ITEM_H_INCLUDED */

