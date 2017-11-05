#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/thing.h"

typedef enum
{
	EV_NONE = 0,
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
	EV_MKILLM,
	EV_MCORPSE,
	EV_MTURN,
	EV_MGEN,
	EV_MREGEN,
	EV_MWIELD,
	EV_MPICKUP,
	EV_MDROP,
	EV_MANGERM,
	EV_MCALM,
	EV_MCHARGE,
	EV_MOPENDOOR,
	EV_MCLOSEDOOR
} EV_TYPE;

typedef union Event
{
	EV_TYPE type;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mwait;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydir, xdir;
	} mmove;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mdomove;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mevade;
	struct
	{
		EV_TYPE type;
		TID thID;
	} munevade;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydir, xdir;
	} mshield;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydir, xdir;
	} mdoshield;
	struct
	{
		EV_TYPE type;
		TID thID;
	} munshield;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydir, xdir;
	} mattkm;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mdoattkm;
	struct
	{
		EV_TYPE type;
		TID frID, toID;
	} mkillm;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mcorpse;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mturn;
	struct
	{
		EV_TYPE type;
	} mgen;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mregen;
	struct
	{
		EV_TYPE type;
		TID thID;
		int arm;
		struct Item *it;
	} mwield;
	struct
	{
		EV_TYPE type;
		TID thID;
		Vector things;
	} mpickup;
	struct
	{
		EV_TYPE type;
		TID thID;
		Vector items;
	} mdrop;
	struct
	{
		EV_TYPE type;
		TID frID, toID;
	} mangerm;
	struct
	{
		EV_TYPE type;
		TID thID;
	} mcalm;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydest, xdest;
	} mcharge;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydest, xdest;
	} mopendoor;
	struct
	{
		EV_TYPE type;
		TID thID;
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

