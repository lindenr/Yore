/* drawing.c */

#include "include/drawing.h"
#include "include/graphics.h"
#include <math.h>

/* adapted from wikipedia */
bool bres_draw (int fromy, int fromx, uint8_t *grid, uint8_t *grid_t,
                int (*callback) (struct DLevel *, int, int), int ty, int tx)
{
	if ((!callback) && grid && grid[map_buffer(ty, tx)] == 2)
		return true;
	int dy, dx, sy, sx, err, e2, fy, fx;
	fy = fromy;
	fx = fromx;
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
		if (grid_t[map_buffer(fy, fx)] == 0)
			return false;
		if (grid)
			grid[map_buffer(fy, fx)] = 2;
	}
	if (grid)
		grid[map_buffer(fy, fx)] = 2;
	callback = NULL;
	return true;
}
