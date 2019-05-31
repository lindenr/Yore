#ifndef GENERATE_H_INCLUDED
#define GENERATE_H_INCLUDED

#include "include/all.h"

enum LEVEL_TYPE
{
	LEVEL_NONE = 0,
	LEVEL_NORMAL,
	LEVEL_TOWN,
	LEVEL_MAZE,
	LEVEL_SIM,
	LEVEL_3D,
	LEVEL_END
};

extern char *real_player_name;
void generate_map (int dlevel, enum LEVEL_TYPE);
MonsID mons_gen (int dlevel, int, int32_t);
int is_safe_gen  (int dlevel, int z, int y, int x);

// TODO remove
void generate_auto (double *, int, int, uint16_t*, int, int, double);

MonsID gen_player (int dlevel, int z, int y, int x, char *name);
MonsID gen_mons_in_level (int dlevel, int difficulty);
MonsID gen_mons_near_player (int dlevel, int difficulty);
MonsID gen_boss (int dlevel, int z, int y, int x);

enum MTYPE;
void init_mons (struct Monster_internal *mons, enum MTYPE type);

#endif /* GENERATE_H_INCLUDED */

