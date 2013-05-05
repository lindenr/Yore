#ifndef VISION_H_INCLUDED
#define VISION_H_INCLUDED

#include <stdbool.h>
#include "include/thing.h"
#include "include/dlevel.h"

void bres_start (int, int, uint8_t *, uint8_t *);
void bres_callback (int, int, int (*) (struct DLevel *, int, int));
bool bres_draw (int, int);

#endif /* VISON_H_INCLUDED */
