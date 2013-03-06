#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"
#include "include/graphics.h"

#define NUM_KEYS (sizeof(Keys)/sizeof(*Keys))

/*int Kwait   ();
int Kpickup ();
int Keat    ();
int Ksdrop  ();
int Kmdrop  ();
int Kinv    ();
int Knlook  ();
int Kflook  ();
int Kwield  ();
int Ksave   ();
int Kquit   ();*/

struct KStruct
{
	uint32_t key;
	int (*action) ();
}
extern Keys[];

int key_lookup (glyph);

#endif /* PLAYER_H_INCLUDED */
