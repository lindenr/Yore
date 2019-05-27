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

#define GL_HEALTH    0xF0000003
#define GL_STAMINA   0x0F000005
#define GL_POWER     0x05F00004

#define GL_STR       0xF00000EA
#define FMT_STR       "F00000EA"
#define GL_CON       0x0F0000F0
#define FMT_CON       "0F0000F0"
#define GL_WIS       0x05F0000F
#define FMT_WIS       "05F0000F"
#define GL_AGI       0xF70000AF
#define FMT_AGI       "F70000AF"

#include <stddef.h>
#include <stdint.h>

typedef int MonsID, ItemID;
typedef long long unsigned Tick;
typedef long long unsigned EvID;
typedef uint32_t glyph;

struct Graph;
typedef struct Graph *Graph;

struct Vector;
typedef struct Vector *Vector;

struct DLevel;
typedef struct DLevel DLevel;

struct Monster_internal;
typedef struct Monster_internal Mons;

struct Item_internal;
typedef struct Item_internal Item;

#define VECTOR(type) typedef struct _v_ ## type {type *data; size_t siz, len, mlen;} *V_ ## type

VECTOR(ItemID);
VECTOR(MonsID);
VECTOR(DLevel);
VECTOR(Mons);
VECTOR(Item);
VECTOR(Graph);

typedef enum DTile DTile;

#endif /* ALL_H_INCLUDED */

