#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "SDL/SDL.h"
#include "include/all.h"
#include <stdint.h>

/* Structure of a glyph:
 * A glyph is 32 bits wide. From highest to lowest there is:
 *********************************
 *    what         range   width
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

#define MAP_HEIGHT 100
#define MAP_WIDTH  300
#define MAP_TILES (MAP_HEIGHT*MAP_WIDTH)
#define TXT_TILES (snumy*snumx)

extern int snumy, snumx;
extern int pnumy, pnumx;
extern int cam_yloc, cam_xloc;
extern glyph gr_map[MAP_TILES],    *txt_map;
extern char  gr_change[MAP_TILES], *txt_change;

extern SDL_Surface *screen;

/* Initialisation */
void gr_init    (void);

/* Output */
void gr_move    (int, int);
void gr_movecam (int, int);
void gr_centcam (int, int);

void gr_addch   (glyph);
void gr_mvaddch (int, int, glyph);
void gr_baddch  (int, glyph);

void gr_refresh ();
void gr_frefresh();

void gr_clear   ();

void txt_mvaddch(int, int, glyph);
void txt_baddch (int, glyph);

void txt_mvprint(int, int, const char *, ...);

void txt_box    (int, int, int, int);
void txt_fbox   (int, int, int, int, glyph);

/* Input */
char gr_getch   ();
uint32_t gr_getfullch ();
void gr_getstr  (char *, int);
int  gr_equiv   (uint32_t, uint32_t);

void gr_echo    ();
void gr_noecho  ();

void gr_tout    (int);

/* Misc */
int  gr_buffer  (int, int);
int  txt_buffer (int, int);

int  gr_nearedge(int, int);
void gr_wait    (uint32_t);
void gr_resize  (int, int);

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
