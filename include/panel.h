#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"

#define NUM_TABS 3

void p_init  ();
void p_tab   (int);
void p_msg   (char *, ...);
char p_ask   (char *, char *);
void p_lines (Vector);

extern int p_width, p_height, p_open;

#endif /* PANEL_H_INCLUDED */

