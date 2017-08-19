#ifndef NEW_EVENT_H_INCLUDED
#define NEW_EVENT_H_INCLUDED

#include "include/thing.h"

typedef enum
{
	EV_NONE = 0,
	EV_MMOVE,
	EV_MATTK,
	EV_MKILL,
	EV_MTURN,
	EV_MGEN,
	EV_MREGEN,
	EV_MPICKUP,
	EV_MDROP
} EV_TYPE;

typedef union Event
{
	EV_TYPE type;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydest, xdest;
	} mmove;
	struct
	{
		EV_TYPE type;
		TID thID;
		int ydest, xdest;
	} mattk;
	struct
	{
		EV_TYPE type;
		TID frID, toID;
	} mkill;
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
} *Event;

struct QEv
{
	uint64_t tick;
	union Event ev;
};

typedef uint64_t Tick;
extern Tick curtick;

void ev_init  ();
void ev_loop  ();
void ev_queue (int udelay, union Event ev);
//void ev_do    (Event ev);

#endif /* NEW_EVENT_H_INCLUDED */

