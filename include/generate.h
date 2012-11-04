#ifndef GENERATE_H_INCLUDED
#define GENERATE_H_INCLUDED

#include "include/graphics.h"
#include <stdbool.h>

enum LEVEL_TYPE
{
	LEVEL_NONE = 0,
	LEVEL_NORMAL,
	LEVEL_MINES,
	LEVEL_MAZE,
	LEVEL_END
};

extern char *real_player_name;
void generate_map (int, enum LEVEL_TYPE);
uint32_t mons_gen (int, int, int32_t);
bool is_safe_gen  (int, uint32_t, uint32_t);

#define DOT ACS_BULLET

#endif /* GENERATE_H_INCLUDED */
