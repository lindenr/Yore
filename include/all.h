#if !defined(ALL_H_INCLUDED)
#define ALL_H_INCLUDED

#include <limits.h>
#include "debug.h"

/* Player options go here */

/* operating system: either FOONIX or WINDOWS */
#define WINDOWS

/* #define AMERICAN */
#define USING_COL

/* debugging only */
/* #define LIST_TEST */

/* Int sizes (do not change) */
#if UINT_MAX>>32 == 0
#  define INT_SIZE 32
#elif UINT_MAX>>64 == 0
#  define INT_SIZE 64
#else
#  error unknown int size
#endif

/* These crazy spellings... */
#if !defined(AMERICAN)

#define ARMOUR "armour"
#define COLOUR "colour"

#else /* AMERICAN */

#define ARMOUR "armor"
#define COLOUR "color"

#endif /* !AMERICAN */

#define memclr(a,l) (memset(a,0,l))

#if defined(WINDOWS)
# include <windows.h>
#elif defined(FOONIX)
/* *NIX go here */
#else
#error operating system not defined
#endif /* WINDOWS, FOONIX */

#endif /* ALL_H_INCLUDED */
