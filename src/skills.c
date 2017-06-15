/* skills.c */

#include "include/skills.h"
#include "include/vision.h"

const styp all_skills[] = {
	{SK_NONE,    "",        ""},
	{SK_CHARGE,  "Charge",  "Charge!"}
};

const char *sk_name (Skill sk)
{
	return all_skills[sk->type].name;
}

int chID = 0;
int chargepos (struct DLevel *dlevel, int y, int x)
{
	struct Thing *charger = THIID (chID);
	int dy = y - charger->yloc, dx = x - charger->xloc;
	if (dx*dx > 1 || dy*dy > 1)
	{
		fprintf(stderr, "AAAAAAA\n");
		return 0;
	}
	return mons_move (charger, dy, dx, 0)==1;
}

void sk_charge (struct Thing *th, int y, int x)
{
	chID = th->ID;
	mons_usedturn (th);
	bres_draw (th->yloc, th->xloc, NULL, dlv_attr(th->dlevel), &chargepos, y, x);
}

