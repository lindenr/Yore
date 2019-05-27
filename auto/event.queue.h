//generated!

void ev_queue_world_init (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_player_init (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_world_heartbeat (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_itrot (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mthrow (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_proj_move (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_proj_done (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_proj_hit_barrier (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_proj_hit_monster (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_item_explode (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_line_explode (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mdomove (Tick udelay, union Event ev)
{
	struct QEv *qev = ev_queue_aux (udelay, ev);
	mons_internal (ev.mdomove.monsID)->status.move = (typeof(mons_internal (ev.mdomove.monsID)->status.move)) {qev->ID, qev->tick, ev.mdomove.zdir, ev.mdomove.ydir, ev.mdomove.xdir};
}

void ev_queue_mdoevade (Tick udelay, union Event ev)
{
	struct QEv *qev = ev_queue_aux (udelay, ev);
	mons_internal (ev.mdoevade.monsID)->status.evade = (typeof(mons_internal (ev.mdoevade.monsID)->status.evade)) {qev->ID, qev->tick, ev.mdoevade.zdir, ev.mdoevade.ydir, ev.mdoevade.xdir};
}

void ev_queue_munevade (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mdoshield (Tick udelay, union Event ev)
{
	struct QEv *qev = ev_queue_aux (udelay, ev);
	mons_internal (ev.mdoshield.monsID)->status.shield = (typeof(mons_internal (ev.mdoshield.monsID)->status.shield)) {qev->ID, qev->tick, ev.mdoshield.ydir, ev.mdoshield.xdir};
}

void ev_queue_munshield (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mdohit (Tick udelay, union Event ev)
{
	struct QEv *qev = ev_queue_aux (udelay, ev);
	mons_internal (ev.mdohit.frID)->status.hit = (typeof(mons_internal (ev.mdohit.frID)->status.hit)) {qev->ID, qev->tick, ev.mdohit.arm, ev.mdohit.zdir, ev.mdohit.ydir, ev.mdohit.xdir};
}

void ev_queue_mpoll (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mgen (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mregen (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mbleed (Tick udelay, union Event ev)
{
	struct QEv *qev = ev_queue_aux (udelay, ev);
	mons_internal (ev.mbleed.monsID)->status.bleed = (typeof(mons_internal (ev.mbleed.monsID)->status.bleed)) {qev->ID, qev->tick};
}

void ev_queue_mwield (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mdowield (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mwear_armour (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mtakeoff_armour (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mpickup (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mdrop (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mstartcharge (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mdocharge (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mstopcharge (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mfireball (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mwater_bolt (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mfrost (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mflash (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mstopflash (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mopendoor (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_mclosedoor (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}

void ev_queue_compute (Tick udelay, union Event ev)
{
	ev_queue_aux (udelay, ev);
}
