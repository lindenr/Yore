#ifndef GENERATE_H_INCLUDED
#define GENERATE_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"
#include "include/dlevel.h"

enum LEVEL_TYPE
{
	LEVEL_NONE = 0,
	LEVEL_NORMAL,
	LEVEL_TOWN,
	LEVEL_MAZE,
	LEVEL_SIM,
	LEVEL_END
};

extern char *real_player_name;
void generate_map (struct DLevel *, enum LEVEL_TYPE);
struct Monster *mons_gen (struct DLevel *, int, int32_t);
int is_safe_gen  (struct DLevel *, int z, int y, int x);

// TODO remove
void generate_auto (double *, int, int, uint16_t*, int, int, double);

struct Monster *gen_player (int z, int y, int x, char *name);
struct Monster *gen_mons_in_level ();
struct Monster *gen_mons_near_player ();
struct Monster *gen_boss (int z, int y, int x);

enum MTYPE;
void init_mons (struct Monster *mons, enum MTYPE type);

#endif /* GENERATE_H_INCLUDED */

