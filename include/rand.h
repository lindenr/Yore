#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"
#include <time.h>
#include <stdlib.h>

#define RNG_INIT(s) (struct RNGStruct){s, {0xb3855687, s^31234, s^((s>>7)+22345), 4674567, s-756785678,0,}}
#define RND(a,b) dice_roll(&RNG_main, (a), (b))
#define RN(n)    even_prob(&RNG_main, (n))

/* Some Sort of pSuedo-Random Time Thingy */
extern uint32_t SSSRTT_32();

extern uint32_t RNG_get_seed();

struct RNGStruct
{
	uint32_t seed;
	uint32_t current[10];
};

/* Random number, simulating rolling a dice with b faces (sides). NOT EVEN -
   biased towards the middle range. */
extern uint32_t dice_roll(struct RNGStruct *, uint32_t, uint32_t);

/* Even-probability from 1 to n (like one n-sided die). */
extern uint32_t even_prob(struct RNGStruct *, uint32_t);

extern struct RNGStruct RNG_main;

#endif /* HASH_H_INCLUDED */
