#include "vision.h"
#include <math.h>

int fromy, fromx;
uint8_t *grid, *grid_t;

void bres_start(int fry, int frx, uint8_t *g, uint8_t *g_t)
{
    fromy = fry;
    fromx = frx;
    grid = g;
    grid_t = g_t;
}

/* adapted from wikipedia */
bool bres_draw(int ty, int tx)
{
    int dy, dx, sy, sx, err, e2, fy, fx;
    fy = fromy;
    fx = fromx;
    dy = fabs(ty-fy);
    dx = fabs(tx-fx);
    if (fx < tx) sx = 1;
    else sx = -1;
    if (fy < ty) sy = 1;
    else sy = -1;
    err = dx-dy;

    while (fy != ty || fx != tx)
    {
        e2 = err<<1;
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
        if (fy == ty && fx == tx) break;
        if (grid_t[to_buffer(fy,fx)] == 0) return false; /* can't see it :( */
    }
    if (grid) grid[to_buffer(fy,fx)] = 2; /* can see it :) */
    return true;
}

