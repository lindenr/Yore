#ifndef SKILL_H_INCLUDED
#define SKILL_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"

#define SK_NAME_LEN 30
#define SK_DESC_LEN 400

enum SK_TYPE
{
	SK_NONE = 0,
	SK_CHARGE,
	SK_NUM
};

/* type of skill */
typedef struct
{
	enum SK_TYPE type;
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
void sk_exp    (struct Thing *, Skill, int);
int sk_lvl     (struct Thing *, enum SK_TYPE);

void sk_charge (struct Thing *, int, int, Skill);

#endif /* SKILL_H_INCLUDED */

