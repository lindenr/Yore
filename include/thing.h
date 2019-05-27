#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "include/all.h"
#include "include/item.h"

void    set_tile       (struct DLevel *, DTile, int w);

MonsID  mons_create    (struct DLevel *, int z, int y, int x, struct Monster_internal *);
void    mons_move      (MonsID, int lvl, int z, int y, int x);
void    mons_destroy   (MonsID);

void    draw_map       (struct DLevel *, MonsID);
void    draw_map_xyz   (struct DLevel *, int z, int y, int x);
void    draw_map_buf   (struct DLevel *, int idx);
void    update_knowledge (MonsID);

glyph   glyph_to_draw  (struct DLevel *, int, int);

ItemID  item_create    (struct Item_internal *, union ItemLoc loc);
void    item_put       (ItemID, union ItemLoc loc);
void    it_destroy     (ItemID);

//void walls_test ();

#endif /* THING_H_INCLUDED */

