#ifndef PACK_H_INCLUDED
#define PACK_H_INCLUDED

#include "include/item.h"
#include <stdbool.h>
#include "include/graphics.h"

#define MAX_ITEMS_IN_PACK 52
#define MAX_ITEMS_IN_BAG  52

#define FLAG_DOPRINT 8

/* max 52 items (bounded by weight) possibly extend this to # (53) as well? */
struct Pack
{
	struct Item *items[MAX_ITEMS_IN_PACK];
};

/* max 52 items (bounded by weight) */
struct Bag
{
	struct Item *items[MAX_ITEMS_IN_BAG];
};

void pack_get_letters   (struct Pack, char *);
struct Item *pack_rem   (struct Pack *, char);
bool pack_add           (struct Pack *, struct Item *);
unsigned PACK_AT        (char);
char show_contents      (struct Pack, uint32_t, char *);
struct Item *get_Item   (struct Pack, unsigned);
struct Item *get_Itemc  (struct Pack, char);
char get_Itref          (struct Pack, const struct Item *);

#endif /* PACK_H_INCLUDED */
