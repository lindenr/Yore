#if !defined(ALL_H_INCLUDED)
#define ALL_H_INCLUDED

/* Player options go here */

//#define DEBUG_GETCH_TIME // print time between getch's
//#define DEBUG_GETCH_TIME_GR // print time between getch's in game
//#define DEBUG_REFRESH_TIME // print framerate info
//#define DEBUG_REFRESH_TIME_GR // print framerate info in game
//#define TWOPLAYER // two player characters
//#define FAKE_INPUT // spoofs input for profiling

//#define SIM

/* #define AMERICAN */
#define USING_COL

#define MAX_BOX_LENGTH 40
#define IT_DESC_LEN 128

#define COL_STATUS     0xBBB22200
#define COL_PANEL      0xBBB00020
#define COL_SKILLS     0xBBB11100

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

