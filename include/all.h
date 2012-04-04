#if !defined(ALL_H_INCLUDED)
#define ALL_H_INCLUDED

#include "debug.h"

/* Player options go here */

/* operating system: either FOONIX or WINDOWS */
#define FOONIX

/* #define AMERICAN */
#define USING_COL

/* debugging only */
/* #define LIST_TEST */

/* Permanent - DO NOT CHANGE */
#define INT_32 ((sizeof(int)<<3) == 32)
#define INT_64 ((sizeof(int)<<3) == 64)

/* These crazy spellings... */
#if !defined(AMERICAN)

#define ARMOUR "armour"
#define COLOUR "colour"

#else /* AMERICAN */

#define ARMOUR "armor"
#define COLOUR "color"

#endif /* !AMERICAN */

#endif /* ALL_H_INCLUDED */
