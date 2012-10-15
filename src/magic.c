/* magic.c */

#include "include/magic.h"

char all_spells[256] = {0,};

void magic_spell (struct Monster *mons, char c)
{
}

bool magic_isspell (char c)
{
	return (bool)all_spells[c];
}
