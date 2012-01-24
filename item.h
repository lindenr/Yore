#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include <stdint.h>

#include "thing.h"

enum ITEM_TYPE
{
	IT_LONGSWORD,
	IT_AXE,
	IT_DAGGER,
	IT_SHORTSWORD,
	IT_CHEST,
	IT_GLOVES,
    IT_CORPSE
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

#define ITEM_WEAPON  ')'
#define ITEM_TOOL    '('
#define ITEM_STRANGE ']'
#define ITEM_ARMOUR  '['
#define ITEM_CORPSE  '%'

#define ITEM_INIT = {0,0,""}

/* type of item */
struct item_struct
{
	char     name[20]; /* short name - must be multiple of 4 because using uint32_t for comparison */
	char     ch;       /* for the display */
	enum     ITEM_TYPE type;
	uint32_t wt;       /* weight */
	uint32_t attr;     /* can be used for damage (weapons) (8 bits) */
	uint32_t col;
};

/* an actual physical item */
struct Item
{
	int32_t  type;
	uint32_t attr;
	char    *name;
};

extern struct item_struct items[];

uint32_t find_corpse  (const char *);
char    *get_item_desc(struct Item);
void     item_look    (struct Item*);
char    *get_inv_line (struct Item*);

#endif /* ITEM_H_INCLUDED */
