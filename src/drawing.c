/* drawing.c */

#include "include/drawing.h"
#include "include/graphics.h"
#include <math.h>

/* adapted from wikipedia */
bool bres_draw (int fy, int fx, int ty, int tx, int width,
				uint8_t *grid, uint8_t *grid_t, int (*callback) (int, int))
{
	if ((!callback) && grid && grid[ty*width + tx] == 2)
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
			if (!callback (fy, fx))
				return false;
			continue;
		}
		if (fy == ty && fx == tx)
			break;
		if (grid)
			grid[fy*width + fx] = 2;
		if (grid_t[fy*width + fx] == 0)
			return false;
	}
	if (grid)
		grid[fy*width + fx] = 2;
	return true;
}

void bres_init (struct BresState *st, int fy, int fx, int ty, int tx)
{
	st->dy = abs(ty - fy);
	st->dx = abs(tx - fx);
	st->sx = (fx < tx) - (fx > tx);
	st->sy = (fy < ty) - (fy > ty);
	st->err = st->dx - st->dy;
	st->cy = fy;
	st->cx = fx;
	st->ty = ty;
	st->tx = tx;
	st->done = (st->cy == st->ty) && (st->cx == st->tx);
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
	st->done = (st->cy == st->ty) && (st->cx == st->tx);
}

