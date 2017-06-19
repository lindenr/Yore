#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "SDL/SDL.h"
#include "include/all.h"
#include <stdint.h>

/* Structure of a glyph:
 * A glyph is 32 bits wide. From highest to lowest there is:
 **********************************
 *      what        range   width
 * COL_TXT_RED      0 -  F (4 bits)
 * COL_TXT_GREEN    0 -  F (4 bits)
 * COL_TXT_BLUE     0 -  F (4 bits)
 * COL_BG_RED       0 -  F (4 bits)
 * COL_BG_GREEN     0 -  F (4 bits)
 * COL_BG_BLUE      0 -  F (4 bits)
 * character data  00 - FF (8 bits)
 **********************************/
typedef uint32_t glyph;

#ifdef main
#  undef main
#endif

#define GLW 8
#define GLH 12

typedef struct Graph
{
	int h, w, a;        /* graph dimensions */
	glyph *data;        /* an h*w grid of glyphs */
	uint8_t *flags;     /* tracks changes and boxes */
	int cy, cx;         /* camera location */
	int vy, vx, vh, vw; /* view location and dimensions on the window */
	int vis;            /* whether the graph is currently being shown */
	int csr_y, csr_x;   /* location of the (visible) cursor */
	int csr_state;      /* off, blinking, or steady */
	glyph def;          /* default output colour */
} *Graph;

/* Boxes */
typedef enum
{
	BOX_NONE = 0,
	BOX_HIT,
	BOX_KILL,
	BOX_MAGIC,
	BOX_NUM
} BoxType;

extern int BOXPOS[BOX_NUM][2];
extern int BOXCOL[BOX_NUM][3];

void gra_bsetbox (Graph, int, uint8_t);

/* Prefixes:
 * gr_ is the graphics prefix for generic things to do with the screen
 * gra_ is the graph prefix for messing with a Graph */

extern int gr_h, gr_w, gr_area;
extern int forced_refresh;

//extern void (*gr_onidle)    ();
extern void (*gr_onresize)  ();
//extern void (*gr_onrefresh) ();

extern SDL_Surface *screen;

/* Initialisation */
void gr_init      (void);
Graph gra_init    (int, int, int, int, int, int);
void gra_free     (Graph);

/* Output */
void gra_movecam  (Graph, int, int);
void gra_centcam  (Graph, int, int);

void gra_clear    (Graph);
void gra_invert   (Graph, int, int);

void gra_addch    (Graph, glyph);
void gra_mvaddch  (Graph, int, int, glyph);
void gra_baddch   (Graph, int, glyph);

void gra_box      (Graph, int, int, int, int);
void gra_dbox     (Graph, int, int, int, int);
void gra_fbox     (Graph, int, int, int, int, glyph);

void gra_mvaprint (Graph, int, int, const char *);
void gra_mvprint  (Graph, int, int, const char *, ...);
void gra_cprint   (Graph, int, const char *, ...);

void gr_refresh   ();
void gr_frefresh  ();

void gra_csolid   (Graph);
void gra_cblink   (Graph);
void gra_cmove    (Graph, int, int);
void gra_cshow    (Graph);
void gra_chide    (Graph);

void gra_show     (Graph);
void gra_hide     (Graph);

void gra_mark     (Graph, int, int);

/* Input */
char     gr_getch     ();
uint32_t gr_getfullch ();
void     gra_getstr   (Graph, int, int, char *, int);
int      gr_equiv     (uint32_t, uint32_t);

void gr_tout      (int);

/* Misc */
int  gra_buffer   (Graph, int, int);
int  gr_buffer    (int, int);

int  gra_nearedge (Graph, int, int);
void gr_wait      (uint32_t);
uint32_t gr_getms ();
void gr_resize    (int, int);

/* Unusual characters */
#define CH_BS        0x08
#define CH_TAB       0x09
#define CH_LF        0x0A
#define CH_CR        0x0D
#define CH_ESC       0x1B

#define GRK_UP       0x1E
#define GRK_DN       0x1F
#define GRK_LF       0x11
#define GRK_RT       0x10

#define ACS_BULLET   0xFA

/* line graphics */
#define ACS_VLINE    0xB3
#define ACS_HLINE    0xC4
#define ACS_ULCORNER 0xDA
#define ACS_LLCORNER 0xC0
#define ACS_URCORNER 0xBF
#define ACS_LRCORNER 0xD9
#define ACS_LTEE     0xC3
#define ACS_RTEE     0xB4
#define ACS_TTEE     0xC2
#define ACS_BTEE     0xC1
#define ACS_PLUS     0xC5

/* doulbe-line graphics */
#define DCS_VLINE    0xBA
#define DCS_HLINE    0xCD
#define DCS_ULCORNER 0xC9
#define DCS_LLCORNER 0xC8
#define DCS_URCORNER 0xBB
#define DCS_LRCORNER 0xBC
#define DCS_LTEE     0xCC
#define DCS_RTEE     0xB9
#define DCS_TTEE     0xCB
#define DCS_BTEE     0xCA
#define DCS_PLUS     0xCE

/* Colours. 0 <= n < 16 */
#define COL_TXT_RED(n)   ((n) << 28)
#define COL_TXT_GREEN(n) ((n) << 24)
#define COL_TXT_BLUE(n)  ((n) << 20)
#define COL_BG_RED(n)    ((n) << 16)
#define COL_BG_GREEN(n)  ((n) << 12)
#define COL_BG_BLUE(n)   ((n) <<  8)

/* Common colours */
#define COL_TXT_DEF    0xBBB00000
#define COL_TXT_BRIGHT 0xFFF00000

#endif /* GRAPHICS_H_INCLUDED */

