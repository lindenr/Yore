/* magic.c */

#include "include/magic.h"
#include "include/pline.h"

#include <stdio.h>

#define SPELL(c) (all_spells[(int)c])
uint32_t all_spells[256] = {0,};
void magic_init()
{
	SPELL('f') = SP_FIRE;
	SPELL('a') = SP_AIR;
	SPELL('e') = SP_EARTH;
	SPELL('w') = SP_WATER;
}

void magic_plspell (struct Monster *mons, char c)
{
	pline_col(SPELL(c), "Do what? ");
	getch();
}

bool magic_isspell (char c)
{
	return (bool) (SPELL(c) & SP_ETYPE);
}
