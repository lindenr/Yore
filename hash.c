/* hash.c
 * Linden Ralph, Paul Hsieh */

#include "all.h"
#include "stdint.h"

#include "rand.h"

struct RNGStruct RNG_main;
uint32_t SuperFastHash(const char*, int);

uint32_t dice_roll(struct RNGStruct *r, uint32_t a, uint32_t b)
{
    /* the end product */
    uint32_t ret = 0;
	uint32_t i;
    /* for each of the dice */
    for (i = 0; i < a; ++ i)
    {
        /* roll it and add to the final result */
        ret += even_prob (r, b);
    }
    return ret;
}

uint32_t even_prob(struct RNGStruct *r, uint32_t n)
{
	int i;
	uint32_t *current = r->current;
	for(i = 9; i > 0; -- i) current[i] ^= current[i-1];
	current[0] ^= current[7]+current[2]^current[8]^r->seed;
    r->seed ^= current[9];
    return((r->seed=SuperFastHash((const char*)(current),
                                  sizeof(uint32_t)*10))%n) + 1;
}

uint32_t RNG_get_seed ()
{
    uint32_t tm = SSSRTT_32(), m = clock();
    uint32_t *times = malloc(sizeof(uint32_t)*(10));
	times[0] = tm; times[1] = m; times[8] = 0x86b4e8fd; /* completely arbitrary */
    return SuperFastHash((const char *)times, 40);
}

uint32_t SSSRTT_32 ()
{
    return (clock()<<3)^(CLOCKS_PER_SEC<<2)^(time(NULL)>>1)^(time(NULL)<<3);
}

/* Paul Hsieh */

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

#if !defined (NULL)
#define NULL (void*) 0
#endif /* NULL */

uint32_t SuperFastHash (const char * data, int len) {
    uint32_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (uint16_t)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}
