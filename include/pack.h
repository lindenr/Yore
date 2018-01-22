#ifndef PACK_H_INCLUDED
#define PACK_H_INCLUDED

#include "include/item.h"
#include <stdbool.h>
#include "include/graphics.h"

#define MAX_ITEMS_IN_PACK 52
#define MAX_ITEMS_IN_BAG  52

#define FLAG_DOPRINT 8

/* max 52 items (bounded by weight) possibly extend this to # (53) as well? */
typedef struct Pack
{
	struct Item items[MAX_ITEMS_IN_PACK];
} Pack;

/* max 52 items (bounded by weight) */
struct Bag
{
	struct Item items[MAX_ITEMS_IN_BAG];
};

void pack_get_letters   (Pack, char *);
struct Item *pack_rem   (Pack *, unsigned);
void pack_free          (Pack **);
bool pack_add           (Pack **, struct Item *, int);
unsigned PACK_AT        (char);
char show_contents      (Pack *, uint32_t, char *);
struct Item *get_Itemc  (Pack *, char);
char get_Itref          (const struct Item *item);
Pack *pack_init         ();

#endif /* PACK_H_INCLUDED */

