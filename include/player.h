#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"
#include "include/graphics.h"

#define NUM_KEYS (sizeof(Keys)/sizeof(*Keys))

struct KStruct
{
	uint32_t key;
	int (*action) (struct Thing *);
}
extern Keys[];

int key_lookup (struct Thing *, glyph);
extern int cur_players;

#endif /* PLAYER_H_INCLUDED */

