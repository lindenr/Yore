#ifndef SKILL_H_INCLUDED
#define SKILL_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"

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
	SK_FLAMES,
	SK_NUM
};

/* type of skill */
typedef struct
{
	enum SK_TYPE type;
	int  flags;
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
int  sk_isact  (Skill);
void sk_exp    (struct Monster *, Skill, int);
int  sk_lvl    (struct Monster *, enum SK_TYPE);

void sk_charge (struct Monster *, int, int, Skill);
void sk_flames (struct Monster *);
void sk_flames_overlay (struct Monster *);
void sk_fireball (struct Monster *mons, int yloc, int xloc, Skill skill);
void sk_water_bolt (struct Monster *mons, int yloc, int xloc, Skill skill);

#endif /* SKILL_H_INCLUDED */

