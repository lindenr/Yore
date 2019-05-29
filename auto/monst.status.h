// Generated code - don't change directly or it will be overwritten.

#ifndef monst_status_h
#define monst_status_h

struct
{
	EvID evID;
	Tick due;
	ItemID itemID;
	int ydest;
	int xdest;
} throw;
struct
{
	EvID evID;
	Tick due;
	int zdir;
	int ydir;
	int xdir;
} move;
struct
{
	EvID evID;
	Tick due;
	int zdir;
	int ydir;
	int xdir;
} evade;
struct
{
	EvID evID;
	Tick due;
	int ydir;
	int xdir;
} shield;
struct
{
	EvID evID;
	Tick due;
	int arm;
	int zdir;
	int ydir;
	int xdir;
} hit;
struct
{
	EvID evID;
	Tick due;
} bleed;
struct
{
	EvID evID;
	Tick due;
	int arm;
	ItemID itemID;
} wield;
struct
{
	EvID evID;
	Tick due;
	ItemID itemID;
	size_t offset;
} wear;
struct
{
	EvID evID;
	Tick due;
	ItemID itemID;
} takeoff;
struct
{
	EvID evID;
	Tick due;
	V_ItemID pickup;
} pickup;
struct
{
	EvID evID;
	Tick due;
	V_ItemID items;
} drop;
struct
{
	EvID evID;
	Tick due;
} charge;

#endif /* monst_status_h */

