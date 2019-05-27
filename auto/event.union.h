//generated!
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
		ItemID itemID;
	} itrot;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		ItemID itemID;
		int ydest;
		int xdest;
	} mthrow;
	struct
	{
		EV_TYPE type;
		ItemID itemID;
	} proj_move;
	struct
	{
		EV_TYPE type;
		ItemID itemID;
	} proj_done;
	struct
	{
		EV_TYPE type;
		ItemID itemID;
	} proj_hit_barrier;
	struct
	{
		EV_TYPE type;
		ItemID itemID;
		MonsID monsID;
	} proj_hit_monster;
	struct
	{
		EV_TYPE type;
		ItemID itemID;
		int force;
	} item_explode;
	struct
	{
		EV_TYPE type;
		int dlevel;
		int zloc;
		struct BresState bres;
		int dist;
	} line_explode;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int zdir;
		int ydir;
		int xdir;
	} mdomove;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int zdir;
		int ydir;
		int xdir;
	} mdoevade;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} munevade;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int ydir;
		int xdir;
	} mdoshield;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} munshield;
	struct
	{
		EV_TYPE type;
		MonsID frID;
		int arm;
		int zdir;
		int ydir;
		int xdir;
	} mdohit;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} mpoll;
	struct
	{
		EV_TYPE type;
	} mgen;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} mregen;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} mbleed;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int arm;
		ItemID itemID;
	} mwield;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int arm;
		ItemID itemID;
	} mdowield;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		ItemID itemID;
		size_t offset;
	} mwear_armour;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		ItemID itemID;
	} mtakeoff_armour;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		V_ItemID pickup;
	} mpickup;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		V_ItemID items;
	} mdrop;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} mstartcharge;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} mdocharge;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} mstopcharge;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int ydest;
		int xdest;
		int attk;
	} mfireball;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int ydest;
		int xdest;
		int attk;
	} mwater_bolt;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int zdest;
		int ydest;
		int xdest;
		int radius;
	} mfrost;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int speed;
		Tick duration;
	} mflash;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
	} mstopflash;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int ydest;
		int xdest;
	} mopendoor;
	struct
	{
		EV_TYPE type;
		MonsID monsID;
		int ydest;
		int xdest;
	} mclosedoor;
	struct
	{
		EV_TYPE type;
		ItemID itemID;
	} compute;
