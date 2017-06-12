#ifndef PIXEL_H_INCLUDED
#define PIXEL_H_INCLUDED

#include "include/all.h"

#define BOX_NONE  0
#define BOX_HIT   1
#define BOX_KILL  2
#define BOX_MAGIC 3
#define BOX_NUM   4

struct Box
{
	int yloc, xloc;
	int type;
	int expiry;
};

extern int BOXPOS[BOX_NUM][2];
extern int BOXCOL[BOX_NUM][3];

void px_mvaddbox  (int, int, int, int);
void px_mvrembox  (int, int, int);
void px_showboxes ();

void px_csr       ();

#endif /* PIXEL_H_INCLUDED */

