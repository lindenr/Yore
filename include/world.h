#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include "include/all.h"

struct World
{
	Tick tick;
	char *player_name;
	EvID evID;
	struct Heap *events;
	V_DLevel dlevels;
	V_Item items;
	V_Mons mons;
	Graph map;
};

extern struct World world;

void world_init (char *player_name);

#endif /* WORLD_H_INCLUDED */

