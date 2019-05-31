#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "include/all.h"
#include "include/item.h"

MonsID  mons_create    (int dlevel, int z, int y, int x, struct Monster_internal *);
void    mons_move      (MonsID, int lvl, int z, int y, int x);
void    mons_destroy   (MonsID);

void    draw_map       (int, MonsID);
void    draw_map_xyz   (int, int z, int y, int x);
void    draw_map_buf   (int, int idx);
void    update_knowledge (MonsID);

glyph   glyph_to_draw  (int d, int z, int y, int x, int looking);

ItemID  it_create      (struct Item_internal *, union ItemLoc loc);
void    it_put         (ItemID, union ItemLoc loc);
void    it_destroy     (ItemID);

//void walls_test ();

#endif /* THING_H_INCLUDED */

