/* vision.c */

#include "include/vision.h"
#include "include/graphics.h"
#include <math.h>

int fromy, fromx;
uint8_t *grid, *grid_t;
int (*callback) (struct DLevel *, int, int) = NULL;

void bres_start (int fry, int frx, uint8_t *g, uint8_t *g_t)
{
	fromy = fry;
	fromx = frx;
	grid = g;
	grid_t = g_t;
}

void bres_callback (int yloc, int xloc, int (*x) (struct DLevel *, int, int))
{
	callback = x;
	fromy = yloc;
	fromx = xloc;
}

/* adapted from wikipedia */
bool bres_draw (int ty, int tx)
{
	if ((!callback) && grid && grid[gr_buffer(ty, tx)] == 2)
		return true;
	int dy, dx, sy, sx, err, e2, fy, fx;
	fy = fromy;
	fx = fromx;
	dy = fabs(ty - fy);
	dx = fabs(tx - fx);
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
		e2 = err << 1;
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
			{
				callback = NULL;
				return false;
			}
			continue;
		}
		if (fy == ty && fx == tx)
			break;
		if (grid_t[gr_buffer(fy, fx)] == 0)
			return false;
		if (grid)
			grid[gr_buffer(fy, fx)] = 2;
	}
	if (grid)
		grid[gr_buffer(fy, fx)] = 2;
	callback = NULL;
	return true;
}
