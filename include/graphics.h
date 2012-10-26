#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <stdint.h>

/* Structure of a glyph:
 * A glyph is 32 bits wide. From highest to lowest there is:
 *********************************
 *    what         range   width
 * COL_TXT_RED     0 - F  (4 bits)
 * COL_TXT_GREEN   0 - F  (4 bits)
 * COL_TXT_BLUE    0 - F  (4 bits)
 * COL_BG_RED      0 - F  (4 bits)
 * COL_BG_GREEN    0 - F  (4 bits)
 * COL_BG_BLUE     0 - F  (4 bits)
 * character data  0 - FF (8 bits)
 **********************************/
typedef uint32_t glyph;

#define GLW 8
#define GLH 12

#define MAP_HEIGHT 100
#define MAP_WIDTH  300
#define MAP_TILES (MAP_HEIGHT*MAP_WIDTH)
extern int glnumy, glnumx;
extern glyph gr_map[MAP_TILES];

/* Initialisation */
void gr_init    (void);

/* Output */
void gr_move    (int, int);

void gr_addch   (glyph);
void gr_mvaddch (int, int, glyph);
void gr_baddch  (int, glyph);

void gr_printc  (const char *, ...);
void gr_mvprintc(int, int, const char *, ...);

void gr_refresh ();

void gr_clear   ();

/* Input */
char gr_getch   ();
void gr_getstr  (char *);

void gr_echo    ();
void gr_noecho  ();

/* Misc */
int  to_buffer  (int, int);
void wait_ms    (unsigned);

/* Unusual characters */
#define ACS_BULLET   0xfa
#define ACS_VLINE    0xb3
#define ACS_HLINE    0xc4
#define ACS_ULCORNER 0xda
#define ACS_LLCORNER 0xc0
#define ACS_URCORNER 0xbf
#define ACS_LRCORNER 0xd9
#define ACS_LTEE     0xc3
#define ACS_RTEE     0xb4
#define ACS_TTEE     0xc2
#define ACS_BTEE     0xc1
#define ACS_PLUS     0xc5

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
