#ifndef GENERATE_H_INCLUDED
#define GENERATE_H_INCLUDED

#include "include/graphics.h"
#include "include/dlevel.h"
#include <stdbool.h>

enum LEVEL_TYPE
{
	LEVEL_NONE = 0,
	LEVEL_NORMAL,
	LEVEL_TOWN,
	LEVEL_MAZE,
	LEVEL_END
};

extern char *real_player_name;
void generate_map (struct DLevel *, enum LEVEL_TYPE);
struct Monster *mons_gen (struct DLevel *, int, int32_t);
bool is_safe_gen  (struct DLevel *, uint32_t, uint32_t);

// TODO remove
void generate_auto (double *, int, int, uint16_t*, int, int, double);

struct Monster *gen_player (int y, int x, char *name);
struct Monster *gen_mons_in_level ();
struct Monster *gen_boss (int y, int x);

#endif /* GENERATE_H_INCLUDED */

