#include "vision.h"
#include <math.h>
/*
#define HIGHEST_BIT(x) ((x)&(((-1u)>>1)+1))
#define LINE_THRESHOLD 0.5

bool line_passes_through(int fy, int fx, int ty, int tx, int py, int px)
{
    if ((ty < py && fy < py) ||
    (ty > py && fy > py) ||
    (tx < px && fx < px) ||
    (tx > px && fx > px)) return false;
    return true;
    float d1 = sqrt((ty-fy)*(ty-fy) + (tx-fx)*(tx-fx));
    float d2 = sqrt((ty-py)*(ty-py) + (tx-px)*(tx-px));
    float d3 = sqrt((py-fy)*(py-fy) + (px-fx)*(px-fx));
    return(fabs(d1-d2-d3) < LINE_THRESHOLD);
}*/

int fromy, fromx;
uint8_t *grid, *grid_t;

void bres_start(int fry, int frx, uint8_t *g, uint8_t *g_t)
{
    fromy = fry;
    fromx = frx;
    grid = g;
    grid_t = g_t;
}

void bres_draw(int ty, int tx)
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
        if (grid_t[to_buffer(fy,fx)] == 0) return; /* can't see :( */
    }
    grid[to_buffer(fy,fx)] = 1;
}

