// Generated code - don't change directly or it will be overwritten.

#ifndef event_qdecl_h
#define event_qdecl_h
void ev_queue_dlevel_heartbeat (Tick udelay, int dlevel);
void ev_queue_itrot (Tick udelay, ItemID itemID);
void ev_queue_mthrow (Tick udelay, MonsID monsID, ItemID itemID, int ydest, int xdest);
void ev_queue_proj_move (Tick udelay, ItemID itemID, struct BresState bres, int speed, MonsID frID);
void ev_queue_item_explode (Tick udelay, ItemID itemID, int force);
void ev_queue_line_explode (Tick udelay, int dlevel, int zloc, struct BresState bres, int dist);
void ev_queue_mdomove (Tick udelay, MonsID monsID, int zdir, int ydir, int xdir);
void ev_queue_mdoevade (Tick udelay, MonsID monsID, int zdir, int ydir, int xdir);
void ev_queue_munevade (Tick udelay, MonsID monsID);
void ev_queue_mdoshield (Tick udelay, MonsID monsID, int ydir, int xdir);
void ev_queue_munshield (Tick udelay, MonsID monsID);
void ev_queue_mdohit (Tick udelay, MonsID frID, int arm, int zdir, int ydir, int xdir);
void ev_queue_mpoll (Tick udelay, MonsID monsID);
void ev_queue_mgen (Tick udelay, int dlevel);
void ev_queue_mregen (Tick udelay, MonsID monsID);
void ev_queue_mbleed (Tick udelay, MonsID monsID);
void ev_queue_mwield (Tick udelay, MonsID monsID, int arm, ItemID itemID);
void ev_queue_mdowield (Tick udelay, MonsID monsID, int arm, ItemID itemID);
void ev_queue_mwear_armour (Tick udelay, MonsID monsID, ItemID itemID, size_t offset);
void ev_queue_mtakeoff_armour (Tick udelay, MonsID monsID, ItemID itemID);
void ev_queue_mpickup (Tick udelay, MonsID monsID, V_ItemID pickup);
void ev_queue_mdrop (Tick udelay, MonsID monsID, V_ItemID items);
void ev_queue_mdocharge (Tick udelay, MonsID monsID);
void ev_queue_mstopcharge (Tick udelay, MonsID monsID);
void ev_queue_mfireball (Tick udelay, MonsID monsID, int ydest, int xdest, int attk);
void ev_queue_mwater_bolt (Tick udelay, MonsID monsID, int ydest, int xdest, int attk);
void ev_queue_mfrost (Tick udelay, MonsID monsID, int zdest, int ydest, int xdest, int radius);
void ev_queue_mflash (Tick udelay, MonsID monsID, int speed, Tick duration);
void ev_queue_mstopflash (Tick udelay, MonsID monsID);
void ev_queue_mopendoor (Tick udelay, MonsID monsID, int ydest, int xdest);
void ev_queue_mclosedoor (Tick udelay, MonsID monsID, int ydest, int xdest);
void ev_queue_compute (Tick udelay, ItemID itemID);


#endif /* event_qdecl_h */

