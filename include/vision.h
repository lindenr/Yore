#ifndef VISION_H_INCLUDED
#define VISION_H_INCLUDED

#include <stdbool.h>
#include "include/thing.h"
#include "include/dlevel.h"

bool bres_draw (int, int, uint8_t *, uint8_t *, int (*)(struct DLevel *, int, int), int, int);

#endif /* VISON_H_INCLUDED */
