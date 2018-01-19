/* drawing.c */

#include "include/drawing.h"
#include "include/graphics.h"
#include <math.h>

/* adapted from wikipedia */
bool bres_draw (int fy, int fx, uint8_t *grid, uint8_t *grid_t,
                int (*callback) (struct DLevel *, int, int), int ty, int tx)
{
	if ((!callback) && grid && grid[map_buffer(ty, tx)] == 2)
		return true;
	int dy, dx, sy, sx, err, e2;
	dy = abs(ty - fy);
	dx = abs(tx - fx);
	if (fx < tx)
		sx = 1;
	else
		sx = -1;
	if (fy < ty)
		sy = 1;
	else
		sy = -1;
	err = dx - dy;

	while (fy != ty || fx != tx)
	{
		e2 = err * 2;
		if (e2 > -dy)
		{
			err -= dy;
			fx += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			fy += sy;
		}
		if (callback)
		{
			if (!callback (cur_dlevel, fy, fx))
				return false;
			continue;
		}
		if (fy == ty && fx == tx)
			break;
		if (grid)
			grid[map_buffer(fy, fx)] = 2;
		if (grid_t[map_buffer(fy, fx)] == 0)
			return false;
	}
	if (grid)
		grid[map_buffer(fy, fx)] = 2;
	return true;
}

void bres_init (struct BresState *st, int fy, int fx, int ty, int tx)
{
	st->dy = abs(ty - fy);
	st->dx = abs(tx - fx);
	if (fx < tx)
		st->sx = 1;
	else
		st->sx = -1;
	if (fy < ty)
		st->sy = 1;
	else
		st->sy = -1;
	st->err = st->dx - st->dy;
	st->cy = fy;
	st->cx = fx;
	st->ty = ty;
	st->tx = tx;
}

void bres_iter (struct BresState *st)
{
	int e2 = st->err * 2;
	if (e2 > -st->dy)
	{
		st->err -= st->dy;
		st->cx += st->sx;
	}
	if (e2 < st->dx)
	{
		st->err += st->dx;
		st->cy += st->sy;
	}
}

