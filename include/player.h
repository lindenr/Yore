#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "include/all.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/event.h"

#define NUM_KEYS ((int)(sizeof(Keys)/sizeof(*Keys)))

struct KStruct
{
	char key;                         /* TODO make rebindable */
	int (*action) (struct Monster *);
	int (*cand) (struct Monster *);
	//const char *description;        /* for key-rebinding and in-game help? */
};

extern struct KStruct Keys[];

struct Item *player_use_pack (struct Monster *, char *, uint32_t); /* ask player for an item             */
void ask_items    (const struct Monster *player, Vector, Vector, const char *);

int  p_move       (int *, int *, char);
int  key_lookup   (struct Monster *, char);
void pl_queue     (struct Monster *, union Event);
int  pl_execute   (Tick, struct Monster *, int);
int  pl_take_turn (struct Monster *);
int  pl_attempt_move (struct Monster *, int, int);
void pl_init      ();

int  pl_charge_action (struct Monster *);

void pl_choose_attr_gain (struct Monster *, int);

extern Vector player_actions;
extern int pl_focus_mode;

#endif /* PLAYER_H_INCLUDED */

