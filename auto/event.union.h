// Generated code - don't change directly or it will be overwritten.

#ifndef event_union_h
#define event_union_h
	struct
	{
		Ev_type type;
	} world_init;
	struct
	{
		Ev_type type;
	} player_init;
	struct
	{
		Ev_type type;
	} world_heartbeat;
	struct
	{
		Ev_type type;
		ItemID itemID;
	} itrot;
	struct
	{
		Ev_type type;
		MonsID monsID;
		ItemID itemID;
		int ydest;
		int xdest;
	} mthrow;
	struct
	{
		Ev_type type;
		ItemID itemID;
		struct BresState bres;
		int speed;
		MonsID frID;
	} proj_move;
	struct
	{
		Ev_type type;
		ItemID itemID;
		int force;
	} item_explode;
	struct
	{
		Ev_type type;
		int dlevel;
		int zloc;
		struct BresState bres;
		int dist;
	} line_explode;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int zdir;
		int ydir;
		int xdir;
	} mdomove;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int zdir;
		int ydir;
		int xdir;
	} mdoevade;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} munevade;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int ydir;
		int xdir;
	} mdoshield;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} munshield;
	struct
	{
		Ev_type type;
		MonsID frID;
		int arm;
		int zdir;
		int ydir;
		int xdir;
	} mdohit;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} mpoll;
	struct
	{
		Ev_type type;
	} mgen;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} mregen;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} mbleed;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int arm;
		ItemID itemID;
	} mwield;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int arm;
		ItemID itemID;
	} mdowield;
	struct
	{
		Ev_type type;
		MonsID monsID;
		ItemID itemID;
		size_t offset;
	} mwear_armour;
	struct
	{
		Ev_type type;
		MonsID monsID;
		ItemID itemID;
	} mtakeoff_armour;
	struct
	{
		Ev_type type;
		MonsID monsID;
		V_ItemID pickup;
	} mpickup;
	struct
	{
		Ev_type type;
		MonsID monsID;
		V_ItemID items;
	} mdrop;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} mdocharge;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} mstopcharge;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int ydest;
		int xdest;
		int attk;
	} mfireball;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int ydest;
		int xdest;
		int attk;
	} mwater_bolt;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int zdest;
		int ydest;
		int xdest;
		int radius;
	} mfrost;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int speed;
		Tick duration;
	} mflash;
	struct
	{
		Ev_type type;
		MonsID monsID;
	} mstopflash;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int ydest;
		int xdest;
	} mopendoor;
	struct
	{
		Ev_type type;
		MonsID monsID;
		int ydest;
		int xdest;
	} mclosedoor;
	struct
	{
		Ev_type type;
		ItemID itemID;
	} compute;


#endif /* event_union_h */

