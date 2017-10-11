#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"
#include "include/graphics.h"

#define NUM_KEYS ((int)(sizeof(Keys)/sizeof(*Keys)))

struct KStruct
{
	uint32_t key;
	int (*action) (struct Monster *);
};

extern struct KStruct Keys[];

int key_lookup (struct Monster *, char);
extern int cur_players;

#endif /* PLAYER_H_INCLUDED */

