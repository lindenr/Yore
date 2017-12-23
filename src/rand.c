/* hash.c */

#include "include/all.h"
#include "include/rand.h"

uint32_t dice_roll (uint32_t a, uint32_t b)
{
	/* the end product */
	uint32_t ret = 0;
	uint32_t i;
	/* for each of the dice */
	for (i = 0; i < a; ++i)
	{
		/* roll it and add to the final result */
		ret += rn (b) + 1;
	}
	return ret;
}

