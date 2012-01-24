#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

/* RNG */

#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#define RNG_INIT(s) (struct RNGStruct){(uint32_t)5,{0xb19b00b5, 31234, 22345, 4674567, 756785678}}
#define RND(a,b) dice_roll(&RNG_main, (a), (b))
#define RN(n)    even_prob(&RNG_main, (n))

/* Some Sort of pSuedo-Random Time Thingy */
extern uint32_t SSSRTT_32 ();

extern uint32_t RNG_get_seed ();

struct RNGStruct
{
    uint32_t seed;
    uint32_t current[10];
};

/* Random number, simulating rolling a dice with b faces (sides).
 * NOT EVEN - biased towards the middle range. */
extern uint32_t dice_roll(struct RNGStruct*, uint32_t, uint32_t);

/* Even-probability from 1 to n (like one n-sided die). */
extern uint32_t even_prob(struct RNGStruct*, uint32_t);

extern struct RNGStruct RNG_main;

/* END RNG */

#endif /* HASH_H_INCLUDED */
