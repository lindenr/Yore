#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"
#include "include/graphics.h"

#define NUM_TABS 3

#define PANEL_CLOSED  0
#define PANEL_SELF    1
#define PANEL_MSG     2
#define PANEL_OPTIONS 3

int  sb_buffer (int, int);
void sb_baddch (int, glyph);
void sb_mvaddch(int, int, glyph);
void sb_mvprint(int, int, char *, ...);
void p_sidebar (int);

void p_init    ();
void p_pane    ();
void p_panel   (int);
void p_update  ();
void p_tab     (int);
void p_msg     (char *, ...);
char p_ask     (char *, char *);
void p_lines   (Vector);

extern int p_width, p_height
//, p_open
;

#endif /* PANEL_H_INCLUDED */
