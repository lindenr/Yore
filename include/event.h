#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/thing.h"

typedef enum
{
	EV_NONE = 0,
	EV_WORLD_INIT,
	EV_PLAYER_INIT,
	EV_WORLD_HEARTBEAT,
	EV_MTHROW,
	EV_PROJ_MOVE,
	EV_PROJ_DONE,
	EV_PROJ_HIT_BARRIER,
	EV_PROJ_HIT_MONSTER,
	EV_MWAIT,
	EV_MMOVE,
	EV_MDOMOVE,
	EV_MEVADE,
	EV_MUNEVADE,
//	EV_MPARRY,
	EV_MSHIELD,
	EV_MDOSHIELD,
	EV_MUNSHIELD,
	EV_MATTKM,
	EV_MDOATTKM,
	EV_MTURN,
	EV_MGEN,
	EV_MREGEN,
	EV_MWIELD,
	EV_MWEAR_ARMOUR,
	EV_MTAKEOFF_ARMOUR,
	EV_MPICKUP,
	EV_MDROP,
	EV_MSTARTCHARGE,
	EV_MDOCHARGE,
	EV_MSTOPCHARGE,
	EV_MFIREBALL,
	EV_MWATER_BOLT,
	EV_MFROST,
	EV_CIRCLEOFFLAME,
	EV_MOPENDOOR,
	EV_MCLOSEDOOR
} EV_TYPE;

typedef union Event
{
	EV_TYPE type;
	struct
	{
		EV_TYPE type;
	} world_init;
	struct
	{
		EV_TYPE type;
	} player_init;
	struct
	{
		EV_TYPE type;
	} world_heartbeat;
	struct
	{
		EV_TYPE type;
		MID thID;
		TID itemID;
		int ydest, xdest;
	} mthrow;
	struct
	{
		EV_TYPE type;
		TID itemID;
	} proj_move;
	struct
	{
		EV_TYPE type;
		TID itemID;
	} proj_done;
	struct
	{
		EV_TYPE type;
		TID itemID;
	} proj_hit_barrier;
	struct
	{
		EV_TYPE type;
		TID itemID;
		MID monsID;
	} proj_hit_monster;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mwait;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydir, xdir;
	} mmove;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mdomove;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mevade;
	struct
	{
		EV_TYPE type;
		MID thID;
	} munevade;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydir, xdir;
	} mshield;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydir, xdir;
	} mdoshield;
	struct
	{
		EV_TYPE type;
		MID thID;
	} munshield;
	struct
	{
		EV_TYPE type;
		MID thID;
		int arm;
		int ydir, xdir;
	} mattkm;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mdoattkm;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mturn;
	struct
	{
		EV_TYPE type;
	} mgen;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mregen;
	struct
	{
		EV_TYPE type;
		MID thID;
		int arm;
		TID itemID;
	} mwield;
	struct
	{
		EV_TYPE type;
		MID thID;
		TID itemID;
		size_t offset;
	} mwear_armour;
	struct
	{
		EV_TYPE type;
		MID thID;
		TID itemID;
	} mtakeoff_armour;
	struct
	{
		EV_TYPE type;
		MID thID;
		Vector things;
	} mpickup;
	struct
	{
		EV_TYPE type;
		MID thID;
		Vector items;
	} mdrop;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mstartcharge;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydir, xdir;
	} mdocharge;
	struct
	{
		EV_TYPE type;
		MID thID;
	} mstopcharge;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydest, xdest;
	} mfireball;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydest, xdest;
	} mwater_bolt;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydest, xdest;
		int radius;
	} mfrost;
	struct
	{
		EV_TYPE type;
		//int yloc, xloc;
		//int power, radius;
		MID thID;
	} circleofflame;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydest, xdest;
	} mopendoor;
	struct
	{
		EV_TYPE type;
		MID thID;
		int ydest, xdest;
	} mclosedoor;
} *Event;

struct QEv
{
	Tick tick;
	union Event ev;
};

extern Tick curtick;

void ev_init  ();
void ev_loop  ();
void ev_queue (Tick udelay, union Event ev);
void ev_debug ();
Tick ev_delay (union Event *ev);

#endif /* EVENT_H_INCLUDED */

