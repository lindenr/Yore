#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "include/thing.h"

typedef enum
{
	EV_NONE = 0,
	EV_MRESET,
	EV_MMOVE,
	EV_MDOMOVE,
	EV_MEVADE,
	EV_MATTKM,
	EV_MDOATTKM,
	EV_MKILLM,
	EV_MCORPSE,
	EV_MTURN,
	EV_MGEN,
	EV_MREGEN,
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
	} mreset;
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

typedef uint64_t Tick;
extern Tick curtick;

void ev_init  ();
void ev_loop  ();
void ev_queue (int udelay, union Event ev);
void ev_debug ();

#endif /* EVENT_H_INCLUDED */

