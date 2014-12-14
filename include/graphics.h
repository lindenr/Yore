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

/* note: visible graphs shouldn't overlap, as the drawing order is undefined */
typedef struct Graph
{
	int h, w, a;        /* graph dimensions */
	glyph *data;        /* an h*w grid of glyphs */
	char *change;       /* whether a point has changed since last refresh */
	int cy, cx;         /* camera location */
	int vy, vx, vh, vw; /* view location and dimensions on the window */
	int vis;            /* whether the graph is currently being shown */
} *Graph;

/* Prefixes:
 * gr_ is the graphics prefix for generic things to do with the screen
 * txt_ is the text prefix for managing the text overlay
 * gra_ is the graph prefix for messing with a Graph */

extern int txt_h, txt_w, txt_area;
extern glyph *txt_map;
extern char  *txt_change;
extern int csr_y, csr_x, csr_state;
extern int forced_refresh;

extern void (*gr_onidle)    ();
extern void (*gr_onresize)  ();
extern void (*gr_onrefresh) ();

extern SDL_Surface *screen;

/* Initialisation */
void gr_init   (void);
Graph gra_init (int, int, int, int, int, int);

/* Output */
void txt_move    (int, int);
void gra_movecam (Graph, int, int);
void gra_centcam (Graph, int, int);

void gra_addch   (Graph, glyph);
void gra_mvaddch (Graph, int, int, glyph);
void gra_baddch  (Graph, int, glyph);

void gr_refresh ();
void gr_frefresh();

void txt_clear ();

void txt_mvaddch (int, int, glyph);
void txt_baddch  (int, glyph);

void txt_mvprint (int, int, const char *, ...);

void txt_box  (int, int, int, int);
void txt_dbox (int, int, int, int);
void txt_fbox (int, int, int, int, glyph);

void csr_noblink ();
void csr_blink   ();
void csr_move    (int, int);
void csr_show    ();
void csr_hide    ();

void txt_mark (int, int);
void gra_mark (Graph, int, int);

/* Input */
char     gr_getch     ();
uint32_t gr_getfullch ();
void     txt_getstr   (char *, int);
int      gr_equiv     (uint32_t, uint32_t);

int txt_echo   (int);

void gr_tout (int);

/* Misc */
int  gra_buffer (Graph, int, int);
int  txt_buffer (int, int);

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

