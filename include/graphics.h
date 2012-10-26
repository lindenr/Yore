#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <stdint.h>
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

void gr_echo    ();
void gr_noecho  ();

void wait_ms    (unsigned);

#endif /* GRAPHICS_H_INCLUDED */
