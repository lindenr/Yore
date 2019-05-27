#ifndef SKILL_H_INCLUDED
#define SKILL_H_INCLUDED

#include "include/all.h"

#define SK_NAME_LEN 30
#define SK_DESC_LEN 400

#define SK_ACT 1
#define SK_PAS 2

enum SK_TYPE
{
	SK_NONE = 0,
	SK_CHARGE,
	SK_DODGE,
	SK_FIREBALL,
	SK_WATER_BOLT,
	SK_FROST,
	SK_SCRY,
	SK_FLASH,
	SK_USE_MARTIAL_ARTS,
	SK_USE_LONGSWORD,
	SK_USE_AXE,
	SK_USE_HAMMER,
	SK_USE_DAGGER,
	SK_USE_SHORTSWORD,
	SK_NUM
};

/* type of skill */
typedef struct
{
	enum SK_TYPE type;
	int  flags;
	glyph gl;
	char name[SK_NAME_LEN];
	char desc[SK_DESC_LEN];
} styp;

/* skill possessed by a monster */
typedef struct Skill
{
	enum SK_TYPE type;
	int exp;
	int level;
} *Skill;

const char *sk_name (Skill);
const char *sk_desc (Skill);
glyph       sk_gl   (Skill);
int  sk_isact  (Skill);
void sk_exp    (MonsID, Skill, int);
int  sk_lvl    (MonsID, enum SK_TYPE);

void sk_charge (MonsID mon, int, int, Skill);
void sk_fireball (MonsID mons, int yloc, int xloc, Skill skill);
void sk_water_bolt (MonsID mons, int yloc, int xloc, Skill skill);
void sk_frost (MonsID mons, int yloc, int xloc, Skill skill);
void sk_scry (MonsID mons, Skill skill);
void sk_flash (MonsID mons, Skill skill);

#endif /* SKILL_H_INCLUDED */

