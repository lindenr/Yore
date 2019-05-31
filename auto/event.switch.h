// Generated code - don't change directly or it will be overwritten.

#ifndef event_switch_h
#define event_switch_h

void ev_do (const struct QEv *qev)
{
	const union Event *ev = &qev->ev;
	switch (ev->type)
	{
		case EV_dlevel_heartbeat:
		{
			ev_dlevel_heartbeat (ev->dlevel_heartbeat.dlevel);
			return;
		}
		case EV_itrot:
		{
			if (it_is (ev->itrot.itemID))
				ev_itrot (ev->itrot.itemID);
			return;
		}
		case EV_mthrow:
		{
			if ((!mons_is (ev->mthrow.monsID)) ||
				qev->ID != mons_internal(ev->mthrow.monsID)->status.throw.evID)
				return;
			if (mons_is (ev->mthrow.monsID))
				mons_internal(ev->mthrow.monsID)->status.throw.evID = 0;
			if (mons_is (ev->mthrow.monsID) && it_is (ev->mthrow.itemID))
				ev_mthrow (ev->mthrow.monsID, ev->mthrow.itemID, ev->mthrow.ydest, ev->mthrow.xdest);
			if (mons_is (ev->mthrow.monsID))
				mons_poll (ev->mthrow.monsID);
			return;
		}
		case EV_proj_move:
		{
			if ((!it_is (ev->proj_move.itemID)) ||
				qev->ID != it_internal(ev->proj_move.itemID)->status.flight.evID)
				return;
			if (it_is (ev->proj_move.itemID))
				it_internal(ev->proj_move.itemID)->status.flight.evID = 0;
			if (it_is (ev->proj_move.itemID))
				ev_proj_move (ev->proj_move.itemID, ev->proj_move.bres, ev->proj_move.speed, ev->proj_move.frID);
			return;
		}
		case EV_item_explode:
		{
			if (it_is (ev->item_explode.itemID))
				ev_item_explode (ev->item_explode.itemID, ev->item_explode.force);
			return;
		}
		case EV_line_explode:
		{
			ev_line_explode (ev->line_explode.dlevel, ev->line_explode.zloc, ev->line_explode.bres, ev->line_explode.dist);
			return;
		}
		case EV_mdomove:
		{
			if ((!mons_is (ev->mdomove.monsID)) ||
				qev->ID != mons_internal(ev->mdomove.monsID)->status.move.evID)
				return;
			if (mons_is (ev->mdomove.monsID))
				mons_internal(ev->mdomove.monsID)->status.move.evID = 0;
			if (mons_is (ev->mdomove.monsID))
				ev_mdomove (ev->mdomove.monsID, ev->mdomove.zdir, ev->mdomove.ydir, ev->mdomove.xdir);
			if (mons_is (ev->mdomove.monsID))
				mons_poll (ev->mdomove.monsID);
			return;
		}
		case EV_mdoevade:
		{
			if ((!mons_is (ev->mdoevade.monsID)) ||
				qev->ID != mons_internal(ev->mdoevade.monsID)->status.evade.evID)
				return;
			if (mons_is (ev->mdoevade.monsID))
				ev_mdoevade (ev->mdoevade.monsID, ev->mdoevade.zdir, ev->mdoevade.ydir, ev->mdoevade.xdir);
			if (mons_is (ev->mdoevade.monsID))
				mons_poll (ev->mdoevade.monsID);
			return;
		}
		case EV_munevade:
		{
			if ((!mons_is (ev->munevade.monsID)) ||
				qev->ID != mons_internal(ev->munevade.monsID)->status.evade.evID)
				return;
			if (mons_is (ev->munevade.monsID))
				mons_internal(ev->munevade.monsID)->status.evade.evID = 0;
			if (mons_is (ev->munevade.monsID))
				ev_munevade (ev->munevade.monsID);
			if (mons_is (ev->munevade.monsID))
				mons_poll (ev->munevade.monsID);
			return;
		}
		case EV_mdoshield:
		{
			if ((!mons_is (ev->mdoshield.monsID)) ||
				qev->ID != mons_internal(ev->mdoshield.monsID)->status.shield.evID)
				return;
			if (mons_is (ev->mdoshield.monsID))
				ev_mdoshield (ev->mdoshield.monsID, ev->mdoshield.ydir, ev->mdoshield.xdir);
			if (mons_is (ev->mdoshield.monsID))
				mons_poll (ev->mdoshield.monsID);
			return;
		}
		case EV_munshield:
		{
			if ((!mons_is (ev->munshield.monsID)) ||
				qev->ID != mons_internal(ev->munshield.monsID)->status.shield.evID)
				return;
			if (mons_is (ev->munshield.monsID))
				mons_internal(ev->munshield.monsID)->status.shield.evID = 0;
			if (mons_is (ev->munshield.monsID))
				ev_munshield (ev->munshield.monsID);
			if (mons_is (ev->munshield.monsID))
				mons_poll (ev->munshield.monsID);
			return;
		}
		case EV_mdohit:
		{
			if ((!mons_is (ev->mdohit.frID)) ||
				qev->ID != mons_internal(ev->mdohit.frID)->status.hit.evID)
				return;
			if (mons_is (ev->mdohit.frID))
				mons_internal(ev->mdohit.frID)->status.hit.evID = 0;
			if (mons_is (ev->mdohit.frID))
				ev_mdohit (ev->mdohit.frID, ev->mdohit.arm, ev->mdohit.zdir, ev->mdohit.ydir, ev->mdohit.xdir);
			if (mons_is (ev->mdohit.frID))
				mons_poll (ev->mdohit.frID);
			return;
		}
		case EV_mpoll:
		{
			if (mons_is (ev->mpoll.monsID))
				ev_mpoll (ev->mpoll.monsID);
			return;
		}
		case EV_mgen:
		{
			ev_mgen (ev->mgen.dlevel);
			return;
		}
		case EV_mregen:
		{
			if (mons_is (ev->mregen.monsID))
				ev_mregen (ev->mregen.monsID);
			return;
		}
		case EV_mbleed:
		{
			if ((!mons_is (ev->mbleed.monsID)) ||
				qev->ID != mons_internal(ev->mbleed.monsID)->status.bleed.evID)
				return;
			if (mons_is (ev->mbleed.monsID))
				mons_internal(ev->mbleed.monsID)->status.bleed.evID = 0;
			if (mons_is (ev->mbleed.monsID))
				ev_mbleed (ev->mbleed.monsID);
			if (mons_is (ev->mbleed.monsID))
				mons_poll (ev->mbleed.monsID);
			return;
		}
		case EV_mwield:
		{
			if ((!mons_is (ev->mwield.monsID)) ||
				qev->ID != mons_internal(ev->mwield.monsID)->status.wield.evID)
				return;
			if (mons_is (ev->mwield.monsID) && it_is (ev->mwield.itemID))
				ev_mwield (ev->mwield.monsID, ev->mwield.arm, ev->mwield.itemID);
			if (mons_is (ev->mwield.monsID))
				mons_poll (ev->mwield.monsID);
			return;
		}
		case EV_mdowield:
		{
			if ((!mons_is (ev->mdowield.monsID)) ||
				qev->ID != mons_internal(ev->mdowield.monsID)->status.wield.evID)
				return;
			if (mons_is (ev->mdowield.monsID))
				mons_internal(ev->mdowield.monsID)->status.wield.evID = 0;
			if (mons_is (ev->mdowield.monsID) && it_is (ev->mdowield.itemID))
				ev_mdowield (ev->mdowield.monsID, ev->mdowield.arm, ev->mdowield.itemID);
			if (mons_is (ev->mdowield.monsID))
				mons_poll (ev->mdowield.monsID);
			return;
		}
		case EV_mwear_armour:
		{
			if ((!mons_is (ev->mwear_armour.monsID)) ||
				qev->ID != mons_internal(ev->mwear_armour.monsID)->status.wear.evID)
				return;
			if (mons_is (ev->mwear_armour.monsID))
				mons_internal(ev->mwear_armour.monsID)->status.wear.evID = 0;
			if (mons_is (ev->mwear_armour.monsID) && it_is (ev->mwear_armour.itemID))
				ev_mwear_armour (ev->mwear_armour.monsID, ev->mwear_armour.itemID, ev->mwear_armour.offset);
			if (mons_is (ev->mwear_armour.monsID))
				mons_poll (ev->mwear_armour.monsID);
			return;
		}
		case EV_mtakeoff_armour:
		{
			if ((!mons_is (ev->mtakeoff_armour.monsID)) ||
				qev->ID != mons_internal(ev->mtakeoff_armour.monsID)->status.takeoff.evID)
				return;
			if (mons_is (ev->mtakeoff_armour.monsID))
				mons_internal(ev->mtakeoff_armour.monsID)->status.takeoff.evID = 0;
			if (mons_is (ev->mtakeoff_armour.monsID) && it_is (ev->mtakeoff_armour.itemID))
				ev_mtakeoff_armour (ev->mtakeoff_armour.monsID, ev->mtakeoff_armour.itemID);
			if (mons_is (ev->mtakeoff_armour.monsID))
				mons_poll (ev->mtakeoff_armour.monsID);
			return;
		}
		case EV_mpickup:
		{
			if ((!mons_is (ev->mpickup.monsID)) ||
				qev->ID != mons_internal(ev->mpickup.monsID)->status.pickup.evID)
				return;
			if (mons_is (ev->mpickup.monsID))
				mons_internal(ev->mpickup.monsID)->status.pickup.evID = 0;
			if (mons_is (ev->mpickup.monsID))
				ev_mpickup (ev->mpickup.monsID, ev->mpickup.pickup);
			if (mons_is (ev->mpickup.monsID))
				mons_poll (ev->mpickup.monsID);
			return;
		}
		case EV_mdrop:
		{
			if ((!mons_is (ev->mdrop.monsID)) ||
				qev->ID != mons_internal(ev->mdrop.monsID)->status.drop.evID)
				return;
			if (mons_is (ev->mdrop.monsID))
				mons_internal(ev->mdrop.monsID)->status.drop.evID = 0;
			if (mons_is (ev->mdrop.monsID))
				ev_mdrop (ev->mdrop.monsID, ev->mdrop.items);
			if (mons_is (ev->mdrop.monsID))
				mons_poll (ev->mdrop.monsID);
			return;
		}
		case EV_mdocharge:
		{
			if ((!mons_is (ev->mdocharge.monsID)) ||
				qev->ID != mons_internal(ev->mdocharge.monsID)->status.charge.evID)
				return;
			if (mons_is (ev->mdocharge.monsID))
				ev_mdocharge (ev->mdocharge.monsID);
			if (mons_is (ev->mdocharge.monsID))
				mons_poll (ev->mdocharge.monsID);
			return;
		}
		case EV_mstopcharge:
		{
			if ((!mons_is (ev->mstopcharge.monsID)) ||
				qev->ID != mons_internal(ev->mstopcharge.monsID)->status.charge.evID)
				return;
			if (mons_is (ev->mstopcharge.monsID))
				mons_internal(ev->mstopcharge.monsID)->status.charge.evID = 0;
			if (mons_is (ev->mstopcharge.monsID))
				ev_mstopcharge (ev->mstopcharge.monsID);
			if (mons_is (ev->mstopcharge.monsID))
				mons_poll (ev->mstopcharge.monsID);
			return;
		}
		case EV_mfireball:
		{
			if (mons_is (ev->mfireball.monsID))
				ev_mfireball (ev->mfireball.monsID, ev->mfireball.ydest, ev->mfireball.xdest, ev->mfireball.attk);
			if (mons_is (ev->mfireball.monsID))
				mons_poll (ev->mfireball.monsID);
			return;
		}
		case EV_mwater_bolt:
		{
			if (mons_is (ev->mwater_bolt.monsID))
				ev_mwater_bolt (ev->mwater_bolt.monsID, ev->mwater_bolt.ydest, ev->mwater_bolt.xdest, ev->mwater_bolt.attk);
			if (mons_is (ev->mwater_bolt.monsID))
				mons_poll (ev->mwater_bolt.monsID);
			return;
		}
		case EV_mfrost:
		{
			if (mons_is (ev->mfrost.monsID))
				ev_mfrost (ev->mfrost.monsID, ev->mfrost.zdest, ev->mfrost.ydest, ev->mfrost.xdest, ev->mfrost.radius);
			if (mons_is (ev->mfrost.monsID))
				mons_poll (ev->mfrost.monsID);
			return;
		}
		case EV_mflash:
		{
			if ((!mons_is (ev->mflash.monsID)) ||
				qev->ID != mons_internal(ev->mflash.monsID)->status.flash.evID)
				return;
			if (mons_is (ev->mflash.monsID))
				mons_internal(ev->mflash.monsID)->status.flash.evID = 0;
			if (mons_is (ev->mflash.monsID))
				ev_mflash (ev->mflash.monsID, ev->mflash.speed, ev->mflash.duration);
			if (mons_is (ev->mflash.monsID))
				mons_poll (ev->mflash.monsID);
			return;
		}
		case EV_mstopflash:
		{
			if (mons_is (ev->mstopflash.monsID))
				ev_mstopflash (ev->mstopflash.monsID);
			if (mons_is (ev->mstopflash.monsID))
				mons_poll (ev->mstopflash.monsID);
			return;
		}
		case EV_mopendoor:
		{
			if (mons_is (ev->mopendoor.monsID))
				ev_mopendoor (ev->mopendoor.monsID, ev->mopendoor.ydest, ev->mopendoor.xdest);
			if (mons_is (ev->mopendoor.monsID))
				mons_poll (ev->mopendoor.monsID);
			return;
		}
		case EV_mclosedoor:
		{
			if (mons_is (ev->mclosedoor.monsID))
				ev_mclosedoor (ev->mclosedoor.monsID, ev->mclosedoor.ydest, ev->mclosedoor.xdest);
			if (mons_is (ev->mclosedoor.monsID))
				mons_poll (ev->mclosedoor.monsID);
			return;
		}
		case EV_compute:
		{
			if ((!it_is (ev->compute.itemID)) ||
				qev->ID != it_internal(ev->compute.itemID)->status.compute.evID)
				return;
			if (it_is (ev->compute.itemID))
				it_internal(ev->compute.itemID)->status.compute.evID = 0;
			if (it_is (ev->compute.itemID))
				ev_compute (ev->compute.itemID);
			return;
		}
	}
}

#endif /* event_switch_h */

