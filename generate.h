#ifndef GENERATE_H_INCLUDED
#define GENERATE_H_INCLUDED

#include "mycurses.h"
#include "bool.h"

enum LEVEL_TYPE
{
    LEVEL_NONE = 0,
    LEVEL_NORMAL,
    LEVEL_MINES,
    LEVEL_FOREST,
    LEVEL_THE_WOLF,
    LEVEL_THE_RABBIT,
    LEVEL_BRRH, /* Big Red Riding Hood */
    LEVEL_BLIND_MICE,
    LEVEL_END
};

void     generate_map (enum LEVEL_TYPE);
uint32_t mons_gen     (int, int32_t);
bool     is_safe_gen  (uint32_t, uint32_t);

#define DOT ACS_BULLET

#endif /* GENERATE_H_INCLUDED */
