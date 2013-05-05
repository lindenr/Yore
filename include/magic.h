#ifndef MAGIC_H_INCLUDED
#define MAGIC_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"
#include "include/vector.h"

#include <stdbool.h>

struct Thing;
#define NUM_SPELLS (sizeof(all_spells)/sizeof(*all_spells))
typedef char *Rune;

struct Spelltype
{
	char *name;
	Vector runes;
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

void sp_shield        (struct Thing *, int, int);
void sp_player_shield ();
int  sp_protected     (struct Thing *, int, int);

void sp_init ();
Rune sp_rune (int);
void sp_tick ();

void pl_cast ();

extern struct Spelltype all_spells[];
extern Vector sp_list;
extern Vector pl_runes;

#endif /* MAGIC_H_INCLUDED */

