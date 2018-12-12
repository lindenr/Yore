#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "SDL.h"
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
typedef uint32_t gflags;

#ifdef main
#  undef main
#endif

#define GLW 8
#define GLH 12

typedef struct Graph
{
	int t, h, w, A, v;  /* graph dimensions: thickness, height, width, area=h*w, volume=h*w*t */
	int glh, glw;       /* glyph dimensions */
	int gldy, gldx;     /* change in y, x pixel coords when going up one level; must be nonpositive */
	glyph *data;//, *old;  /* h*w*t grid of glyphs */
	gflags *flags;      /* tracks (changes and) subpixel options */
	int cpy, cpx, cz, ct; /* camera location and depth */
	int vpy, vpx, vph, vpw; /* view location and dimensions on the window */
	int vis;            /* whether the graph is currently being shown */
	int csr_b;          /* location of the cursor */
	int csr_state;      /* off, (blinking,) or steady */
	glyph def;          /* default output glyph */
} *Graph;

/* Prefixes:
 * gr_ is the graphics prefix for generic things to do with the screen;
 * grx_ is the graph prefix for messing with a Graph fully;
 * gra_ for just in 2d */

extern int gr_ph, gr_pw;//, gr_area;

//extern void (*gr_onidle) ();
extern void (*gr_onresize) ();
//extern void (*gr_onrefresh) ();
extern void (*gr_quit) ();

/* Initialisation */
void gr_init      (int ph, int pw);

Graph grx_init (int t, int h, int w, int glh, int glw, int gldy, int gldx,
	int vpy, int vpx, int vph, int vpw, int ct);
Graph gra_init (int h, int w, int vy, int vx, int vw, int vh);
Graph gra_cinit (int h, int w);
void grx_free (Graph);
#define gra_free(g) (grx_free ((g)))

/* Output */
void grx_movecam (Graph, int z, int y, int x, int t);
#define gra_movecam(g,y,x) (grx_movecam ((g), 0, (y), (x), 1))
void grx_centcam (Graph, int z, int y, int x);
#define gra_centcam(g,y,x) (grx_centcam ((g), 0, (y), (x)))

void grx_clear (Graph);
#define gra_clear(g) (grx_clear ((g)))
void grx_invert (Graph, int z, int y, int x);
#define gra_invert(g,y,x) (grx_invert ((g), 0, (y), (x)))
glyph grx_glinvert (Graph, glyph);
#define gra_glinvert(g,gl) (grx_invert ((g), (gl)))

void grx_mvaddch (Graph, int z, int y, int x, glyph);
#define gra_mvaddch(g,y,x,gl) (grx_mvaddch ((g), 0, (y), (x), (gl)))
void grx_baddch (Graph, int, glyph);
#define gra_baddch(g,i,gl) (grx_baddch ((g), (i), (gl)))
void grx_bgaddch (Graph, int, glyph);
#define gra_bgaddch(g,i,gl) (grx_bgaddch ((g), (i), (gl)))

void grx_box_aux (Graph, int zloc, int yloc, int xloc, int, int,
	glyph, glyph, glyph, glyph, glyph, glyph);
#define gra_box_aux(g,...) (grx_box_aux ((g), 0, __VA_ARGS__))
void grx_box (Graph, int z, int y, int x, int h, int w);
#define gra_box(g,...) (grx_box ((g), 0, __VA_ARGS__))
void grx_dbox (Graph, int z, int y, int x, int h, int w);
#define gra_dbox(g,...) (grx_dbox ((g), 0, __VA_ARGS__))
void grx_fbox (Graph, int z, int y, int x, int h, int w, glyph);
#define gra_fbox(g,...) (grx_fbox ((g), 0, __VA_ARGS__))

void grx_mvaprintex (Graph, int z, int y, int x, const glyph *);
#define gra_mvaprintex(g,...) (grx_mvaprintex ((g), 0, __VA_ARGS__))

void grx_mvaprint (Graph, int z, int y, int x, const char *);
#define gra_mvaprint(g,y,x,s) (grx_mvaprint ((g), 0, (y), (x), (s)))
void grx_mvprint (Graph, int z, int y, int x, const char *, ...);
#define gra_mvprint(g,y,x,...) (grx_mvprint ((g), 0, (y), (x), __VA_ARGS__))
void grx_cprint (Graph, int z, int y, const char *, ...);
#define gra_cprint(g,y,...) (grx_cprint ((g), 0, (y), __VA_ARGS__))

void grx_drawline (Graph, int z, int, int, int, int, glyph);
#define gra_drawline(g,y1,x1,y2,x2,gl) \
(grx_drawline ((g), 0, (y1), (x1), (y2), (x2), (gl)))
void grx_drawdisc (Graph, int z, int y, int x, int r, glyph);
#define gra_drawdisc(g,y,x,r,gl) \
(grx_drawdisc ((g), 0, (y), (x), (r), (gl)))

void gr_refresh   ();

void grx_csolid (Graph);
#define gra_csolid(g) (grx_csolid ((g)))
void grx_cblink (Graph);
#define gra_cblink(g) (grx_cblink ((g)))
void grx_cmove (Graph, int z, int y, int x);
#define gra_cmove(g,y,x) (grx_cmove ((g), 0, (y), (x)))
void grx_cshow (Graph);
#define gra_cshow(g) (grx_cshow ((g)))
void grx_chide (Graph);
#define gra_chide(g) (grx_chide ((g)))

void grx_show (Graph);
#define gra_show(g) (grx_show ((g)))
void grx_hide (Graph);
#define gra_hide(g) (grx_hide ((g)))
//void grx_resize   (Graph, int, int);

/* Input */
char gr_getch     ();
char gr_getch_text();
char gr_getch_int (int);
void grx_getstr   (Graph, int z, int y, int x, char *, int);
#define gra_getstr(g,y,x,s,i) (grx_getstr ((g), 0, (y), (x), (s), (i)))

/* Misc */
int  grx_index    (Graph, int z, int y, int x);
//int  gr_buffer    (int, int);

void gr_ext       (glyph *, char *, glyph);

//int  grx_nearedge (Graph, int, int);
char gr_wait      (uint32_t, int);
uint32_t gr_getms ();
void gr_resize    (int, int);

/* control-key of a lower-case character */
#define GR_CTRL(ch) ((ch)-96)

/* end-of-input reached */
#define GRK_EOF      0xFF
#undef EOF // causes too many promotion problems

/* arrow keys */
#define GRK_UP       0x1E
#define GRK_DN       0x1F
#define GRK_LT       0xAE
#define GRK_RT       0xAF

/* Unusual input characters */
#define GRK_BS       0x08
#define GRK_TAB      0x09
#define GRK_RET      0x0D
#define GRK_ESC      0x1B

/* Unusual output characters/glyphs */
#define ACS_DOT      0xFA
#define ACS_BIGDOT   0xF9
#define ACS_PILLAR   0x07
#define ACS_CORRIDOR 0xB1
#define ACS_DIMCORRIDOR 0xB0

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
#define ACS_TSTUB    0xA9
#define ACS_BSTUB    0xAA
#define ACS_LSTUB    0xAB
#define ACS_RSTUB    0xAC

/* double-line graphics */
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

/* Colours, 0 <= n < 16 */
#define COL_TXT_RED(n)   (((glyph)(n)) << 28)
#define COL_TXT_GREEN(n) (((glyph)(n)) << 24)
#define COL_TXT_BLUE(n)  (((glyph)(n)) << 20)
#define COL_TXT(r,g,b)   (COL_TXT_RED(r)|COL_TXT_GREEN(g)|COL_TXT_BLUE(b))
#define COL_BG_RED(n)    (((glyph)(n)) << 16)
#define COL_BG_GREEN(n)  (((glyph)(n)) << 12)
#define COL_BG_BLUE(n)   (((glyph)(n)) <<  8)
#define COL_BG(r,g,b)    (COL_BG_RED(r)|COL_BG_GREEN(g)|COL_BG_BLUE(b))

/* Common colours */
#define COL_TXT_DEF    0xBBB00000
#define COL_TXT_BRIGHT 0xFFF00000

/* Bitmasks */
#define COL_TXT_MASK   0xFFF00000
#define COL_BG_MASK    0x000FFF00

#endif /* GRAPHICS_H_INCLUDED */

