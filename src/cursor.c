/* cursor.c */

#include "include/cursor.h"
#include "include/vector.h"

void csr_toggle (int id, int tm)
{
	struct Thing *t = THIID(id);
	struct Cursor *csr = &t->thing.csr;
	if (csr->t_change > tm)
		return;
	csr->t_change = t_cur + csr->t[csr->status];
	csr->status = !csr_status;

	px_csr (t->yloc, t->xloc, 
}

