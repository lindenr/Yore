#ifndef MAGIC_H_INCLUDED
#define MAGIC_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"
#include "include/vector.h"

#include <stdbool.h>

#define MAX_RUNES  10
typedef char *Rune;

struct Spelltype
{
	char *name;
	Rune runes[MAX_RUNES];
	int len;
	void (*player_action) ();
};

enum SPELL
{
	SP_NONE = 0,
	SP_SHIELD = 1
};

struct M_shield
{
	int type;
	int yloc, xloc;
	int turns;
};

union Spell
{
	int type;
	struct M_shield shield;
};

void sp_player_shield ();
Rune sp_rune (int);
void sp_tick ();

void pl_cast ();

extern struct Spelltype all_spells[];
extern Vector sp_list;
extern Vector pl_runes;

/*
#define SP_FIRE  (0x00000001 | COL_TXT_RED(15))
#define SP_WATER (0x00000002 | COL_TXT_BLUE(15))
#define SP_AIR   (0x00000003 | COL_TXT_BRIGHT)
#define SP_EARTH (0x00000004 | COL_TXT_GREEN(11) | COL_TXT_RED(11))
#define SP_ETYPE  0x00000007

enum JEWEL_TYPE
{
	/ * Add some power to any spell * /
	JEWEL_1_ = 0,
	JEWEL_2_,
	/ * Add 2 power to an elemental spell * /
	JEWEL_2F, 
	JEWEL_2W,
	JEWEL_2E,
	JEWEL_2A,
	/ * Add 4 power to an elemental spell * /
	JEWEL_4F,
	JEWEL_4W,
	JEWEL_4E,
	JEWEL_4A,
	NUM_JEWELS / * How many jewels; also a placeholder for no jewel * /
};

enum SP_TYPE
{
	SP_ATKF = 0,
	SP_ATKW,
	SP_ATKA,
	SP_ATKE,
	SP_DEFF,
	SP_DEFW,
	SP_DEFA,
	SP_DEFE
};

struct Spell
{
	char ch;
	enum SP_TYPE type;
	int power;
};

bool magic_plspell (char);
bool magic_isspell (char);
*/
#endif /* MAGIC_H_INCLUDED */
