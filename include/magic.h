#ifndef MAGIC_H_INCLUDED
#define MAGIC_H_INCLUDED

#include "include/all.h"
#include "include/monst.h"
#include "include/bool.h"
#include "include/mycurses.h"

#define SP_FIRE  (0x00000001 | COL_TXT_RED | COL_TXT_BRIGHT)
#define SP_WATER (0x00000002 | COL_TXT_BLUE | COL_TXT_BRIGHT)
#define SP_AIR   (0x00000003 | COL_TXT_BRIGHT)
#define SP_EARTH (0x00000004 | COL_TXT_GREEN | COL_TXT_RED)
#define SP_ETYPE 0x00000007

void magic_init    (void);
void magic_plspell (struct Monster *, char);
bool magic_isspell (char);

#endif /* MAGIC_H_INCLUDED */