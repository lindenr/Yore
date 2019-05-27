#ifndef PACK_H_INCLUDED
#define PACK_H_INCLUDED

#include "include/all.h"

#define MAX_ITEMS_IN_PACK 52
#define MAX_ITEMS_IN_BAG  52

#define FLAG_DOPRINT 8

/* max 52 items (bounded by weight) possibly extend this to # (53) as well? */
typedef struct Pack
{
	ItemID items[MAX_ITEMS_IN_PACK];
} Pack;

/* max 52 items (bounded by weight) */
struct Bag
{
	ItemID items[MAX_ITEMS_IN_BAG];
};

void pack_get_letters   (Pack, char *);
void pack_rem           (Pack *, unsigned);
//void pack_free          (Pack **);
int pack_add            (Pack *, ItemID, int);
int PACK_AT             (char);
ItemID show_contents    (MonsID, uint32_t, char *);
ItemID get_Itemc        (Pack *, char);
char get_Itref          (ItemID item);
Pack *pack_init         ();

#endif /* PACK_H_INCLUDED */

