#ifndef RAND_H_INCLUDED
#define RAND_H_INCLUDED

#include "include/all.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#define rng_init()  srand(time(0))
#define rnd(a,b)    dice_roll((a), (b))
#define rn(n)       (rand()%(n))

/* Random number, simulating rolling a dice with b faces (sides). NOT EVEN -
   biased towards the middle range. */
extern uint32_t dice_roll (uint32_t, uint32_t);

#endif /* RAND_H_INCLUDED */

