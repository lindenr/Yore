#if !defined(ALL_H_INCLUDED)
#define ALL_H_INCLUDED

#include "include/debug.h"

/* Player options go here */

//#define DEBUG_REFRESH_TIME // print framerate info
//#define TWOPLAYER          // two player characters


/* #define AMERICAN */
#define USING_COL

#define MAX_BOX_LENGTH 40

#define COL_STATUS     0x00055A00
#define COL_PANEL      0xBBB00000
#define COL_SKILLS     0x000A5500

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
//#error operating system not defined
#endif /* WINDOWS, FOONIX */

#define YORE_VERSION "0.0.7 pre-alpha"

#endif /* ALL_H_INCLUDED */

