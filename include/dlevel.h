#ifndef DLEVEL_H_INCLUDED
#define DLEVEL_H_INCLUDED

#include "include/all.h"

enum DTile
{
	DGN_GROUND = 0,
	DGN_GRASS1,
	DGN_GRASS2,
	DGN_WALL,
	DGN_WALL1,
	DGN_WALL2,
	DGN_ROCK,
	DGN_AIR,
	DGN_DOWNSTAIR,
	DGN_UPSTAIR,
	DGN_TREE,
	DGN_FLOWER1,
	DGN_FLOWER2,
	DGN_CORRIDOR,
	DGN_DOOR,
	DGN_OPENDOOR,
	DGN_SLIME
};

struct DTile_type
{
	char name[20];
	glyph gl;
	uint32_t attr;
};

extern struct DTile_type tile_types[];

struct DLevel
{
	int dlevel;
	int t, h, w, a, v;
	DTile *tiles;    // state for each tile
	V_ItemID *itemIDs; // a vector of item IDs for each tile
	MonsID *monsIDs; // a monster ID for each tile (0 = no monster)
	//V_MonsID playerIDs;
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

int dlv_init (int up, int dn, int t, int h, int w);
//void dlv_set  (int);

struct DLevel *dlv_internal (int);
V_ItemID dlv_items  (int dlevel, int z, int y, int x);
uint8_t *dlv_attr   (int);
//int      dlv_dn     (int);
//int      dlv_up     (int);
void     dlv_fill_player_dist (int dlevel);

MonsID dlv_mons (int dlevel, int z, int y, int x);
void   dlv_setmons (int dlevel, int z, int y, int x, MonsID mons);

void   dlv_addplayer (int dlevel, MonsID player);

DTile  dlv_tile (int dlevel, int z, int y, int x);
void   dlv_settile (int dlevel, int z, int y, int x, DTile tile);

int    dlv_num_fires (int dlevel, int z, int y, int x);
void   dlv_set_fires (int dlevel, int z, int y, int x, int nf);

int    dlv_passable (int dlevel, int z, int y, int x);

/* effect a tile burn */
void dlv_tile_burn (int dlevel, int zloc, int yloc, int xloc);

#endif /* DLEVEL_H_INCLUDED */

