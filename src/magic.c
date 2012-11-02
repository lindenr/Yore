/* magic.c */

#include "include/magic.h"
#include "include/pline.h"
#include "include/monst.h"

#include <stdio.h>
#include <malloc.h>

#define NUM_SPELLS ((sizeof(spells) / sizeof(*spells))-1)
#define SPELL(c,t,p) {c,t,p}

struct Spell spells[] = {
	SPELL('f', SP_ATKF, 2),
	SPELL('a', SP_ATKA, 2),
	SPELL('e', SP_ATKE, 2),
	SPELL('w', SP_ATKW, 2),
	SPELL('F', SP_DEFF, 2),
	SPELL('A', SP_DEFA, 2),
	SPELL('E', SP_DEFE, 2),
	SPELL('W', SP_DEFW, 2),
	SPELL(0,   0,       0)
};
/*
	JEWEL_2F, 
	JEWEL_2W,
	JEWEL_2E,
	JEWEL_2A,

	JEWEL_4F,
	JEWEL_4W,
	JEWEL_4E,
	JEWEL_4A,*/

void magic_alter (struct Spell *sp, enum JEWEL_TYPE jwl)
{
	switch (jwl)
	{
		case JEWEL_2_:
			++ sp->power;
		case JEWEL_1_:
			++ sp->power;
			break;
		if (1) printf("sdfg");
		case JEWEL_4F:
		{
			//
		}
	}
}

struct Spell *magic_create (char c)
{
	int i;
	for (i = 0; i < NUM_SPELLS; ++ i)
	{
		if (c == spells[i].ch) break;
	}
	if (i >= NUM_SPELLS) return NULL;
	
	struct Spell *sp = malloc (sizeof(*sp));
	memcpy (sp, &(spells[i]), sizeof(*sp));
	return sp;
}

void magic_apply (struct Thing *from, struct Spell *spell)
{
}

bool magic_plspell (char c)
{
	int i;
	struct Spell *sp = magic_create (c);
	if (sp == NULL) return false;
	for (i = 0; i < BELT_JEWELS; ++ i)
		magic_alter (sp, U.jewel[i]);
	magic_apply (get_player(), sp);
	return true;
}

bool magic_isspell (char c)
{
	int i;
	for (i = 0; i < NUM_SPELLS; ++ i)
	{
		if (c == spells[i].ch) return true;
	}
	return false;
}
