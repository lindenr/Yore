#ifndef DLEVEL_H_INCLUDED
#define DLEVEL_H_INCLUDED

#include "include/all.h"

struct DLevel
{
	int level;
	int t, h, w, a, v;
	DTile *tiles;    // state for each tile
	V_ItemID *itemIDs; // a vector of item IDs for each tile
	MonsID *monsIDs; // a monster ID for each tile (0 = no monster)
	V_MonsID playerIDs;
	int *player_dist;
	int *escape_dist;
	int *num_fires;

/* Output of the bresenham algorithm (drawing.c): 0 if we can't see it (outside 
 * our field of vision); 1 if we remember it; and 2 if we are looking at it.
 * The options are mutually exclusive. */
	uint8_t *seen;

/* At any given point: 0 if we can't see past that square (e.g. wall); 1 if we 
 * can remember it (e.g. sword); and 2 if we can't remember its position (e.g. 
 * monster). The options are mutually exclusive. */
	uint8_t *attr;

/* What to render/describe instead if it turns out that we can't see something */
	glyph *remembered;
//	int *tilemem;

	int uplevel, dnlevel;
};

void dlv_init ();
void dlv_make (int l, int up, int dn, int t, int h, int w);
void dlv_set  (int);

struct DLevel *dlv_lvl (int);
Vector  *dlv_things (int);
V_ItemID*dlv_itemIDs(int);
uint8_t *dlv_attr   (int);
int      dlv_dn     (int);
int      dlv_up     (int);
void     dlv_fill_player_dist (struct DLevel *);
MonsID dlv_mvmons (int level, int z, int y, int x);
int dlv_index (struct DLevel *, int z, int y, int x);

/* effect a tile burn */
void dlv_tile_burn (struct DLevel *dlvl, int zloc, int yloc, int xloc);

extern V_DLevel all_dlevels;
extern V_Item all_items;
extern V_Mons all_mons;
extern int    cur_level;
extern struct DLevel *cur_dlevel;
extern char *player_name;

#endif /* DLEVEL_H_INCLUDED */

