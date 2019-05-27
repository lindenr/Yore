#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "include/all.h"

#define NUM_KEYS ((int)(sizeof(Keys)/sizeof(*Keys)))

struct KStruct
{
	char key;                         /* TODO make rebindable */
	int (*action) (MonsID);
	int (*cand) (MonsID);
	//const char *description;        /* for key-rebinding and in-game help? */
};

extern struct KStruct Keys[];

ItemID player_use_pack (MonsID, char *, uint32_t); /* ask player for an item             */
void ask_items    (MonsID player, V_ItemID, V_ItemID, const char *);

int  p_move       (int *, int *, char);
int  key_lookup   (MonsID, char);
void pl_poll      (MonsID);
int  pl_attempt_move (MonsID, int, int);
void pl_init      ();

int  pl_charge_action (MonsID);

void pl_choose_attr_gain (MonsID, int);

extern Vector player_actions;
extern int pl_focus_mode;

#endif /* PLAYER_H_INCLUDED */

