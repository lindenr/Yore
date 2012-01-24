#ifndef GENERATE_H_INCLUDED
#define GENERATE_H_INCLUDED

#include "mycurses.h"

enum LEVEL_TYPE
{
    LEVEL_NORMAL = 0,
    LEVEL_MINES,
	LEVEL_FOREST,
	LEVEL_THE_WOLF,
	LEVEL_THE_RABBIT,
	LEVEL_BRRH, /* Big Red Riding Hood */
	LEVEL_BLIND_MICE,
    LEVEL_END
};

extern void generate_map(enum LEVEL_TYPE);
#define DOT ACS_BULLET

#endif /* GENERATE_H_INCLUDED */
