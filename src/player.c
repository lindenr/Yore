/* player.c */

#include "include/player.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/save.h"
#include "include/item.h"
#include "include/event.h"
#include "include/string.h"
#include "include/graphics.h"
#include "include/monst.h"

static char it_descstr[IT_DESC_LEN];

extern Graph map_graph;
int Kcamup (MonsID player)
{
	//grx_movecam (map_graph, 0, map_graph->cy - 10, map_graph->cx, 0);
	map_graph->gldy ++;
	return 0;
}

int Kcamdn (MonsID player)
{
	//grx_movecam (map_graph, 0, map_graph->cy + 10, map_graph->cx, 0);
	map_graph->gldy --;
	return 0;
}

int Kcamlt (MonsID player)
{
	//grx_movecam (map_graph, 0, map_graph->cy, map_graph->cx - 10, 0);
	map_graph->gldx ++;
	return 0;
}

int Kcamrt (MonsID player)
{
	//grx_movecam (map_graph, 0, map_graph->cy, map_graph->cx + 10, 0);
	map_graph->gldx --;
	return 0;
}

int Kstatus (MonsID player)
{
	return p_status (player, P_STATUS);
}

int Kskills (MonsID player)
{
	return p_status (player, P_SKILLS);
}

int Kwait_cand (MonsID player)
{
	return !mons_charging (player);
}

int Kwait (MonsID player)
{
	ev_queue (mons_speed (player)/5, mpoll, player);
	return 1;
}

int Kpickup_cand (MonsID player)
{
	return !mons_charging (player);
}

int Kpickup (MonsID player)
{
	int n = mons_index (player);
	V_ItemID ground = dlv_itemIDs (mons_dlevel (player))[n];

	if (ground->len <= 0)
		return 0;

	V_ItemID pickup = v_dinit (sizeof (ItemID));

	/* One or more items - ask which to pick up. */
	ask_items (player, pickup, ground, "Pick up what?");
	if (pickup->len == 0)
	{
		v_free (pickup);
		return 0;
	}
	ev_queue (mons_speed (player), mpickup, player, pickup);
	return 1;
}

/*int Keat (MonsID player)
{
	ItemID food = player_use_pack ("Eat what?", ITCAT_FOOD);
	if (food == NULL)
		return 0;
	mons_eat (player, food);
	return 1;
}*/

int Kevade (MonsID player)
{
	int ymove, xmove;
	p_notify ("Evade in which direction?");
	char in = p_getch (player);
	p_endnotify ();
	p_move (&ymove, &xmove, in);
	if (ymove == 0 && xmove == 0)
		return 0;
	mons_try_evade (player, ymove, xmove);
	return 1;
}
/*
int Kparry (MonsID player)
{
//	pl_queue (player, mparry, player);
	return 0;
}*/

int Kshield_cand (MonsID player)
{
	return !mons_charging (player);
}

int Kshield (MonsID player)
{
	int ymove, xmove;
	p_notify ("Shield in which direction?");
	char in = p_getch (player);
	p_endnotify ();
	p_move (&ymove, &xmove, in);
	if (ymove == 0 && xmove == 0)
		return 0;
	// TODO mons_try_shield (player, ymove, xmove);?? or are _try_ functions which do
	// nothing much being deprecated?
	ev_queue (0, mdoshield, player, ymove, xmove);
	return 1;
}

int Ksdrop_cand (MonsID player)
{
	return !mons_charging (player);
}

int Ksdrop (MonsID player)
{
	ItemID drop = player_use_pack (player, "Drop what?", ITCAT_ALL);
	if (!drop)
		return 0;

	if (it_worn (drop))
	{
		p_msg ("You're wearing that!");
		return 0;
	}

	V_ItemID vdrop = v_dinit (sizeof(ItemID));
	v_push (vdrop, &drop);

	ev_queue (mons_speed (player), mdrop, player, vdrop);
	return 1;
}

int Kmdrop_cand (MonsID player)
{
	return !mons_charging (player);
}

int Kmdrop (MonsID player)
{
	return 0;
}

int Kfmove_cand (MonsID player)
{
	return 1;
}

int Kfmove (MonsID player)
{
	char in = p_getch (player);
	if (in == GRK_ESC)
		return 0;
	
	int ymove, xmove;
	if (!p_move (&ymove, &xmove, in))
	{
		p_msg ("That's not a direction!");
		return 0;
	}
	mons_try_hit (player, ymove, xmove);
	return 1;
}

int Kgmove_cand (MonsID player)
{
	return 1;
}

int Kgmove (MonsID player)
{
	char in = p_getch (player);
	if (in == GRK_ESC)
		return 0;
	
	int ymove, xmove;
	if (!p_move (&ymove, &xmove, in))
	{
		p_msg ("That's not a direction!");
		return 0;
	}
	mons_try_move (player, 0, ymove, xmove);
	return 1;
}

int Krise (MonsID player)
{
	//if (player->zloc < dlv_lvl (player->dlevel)->t - 1)
	//	mons_move (player, player->dlevel, player->zloc + 1, player->yloc, player->xloc);
	return 0;
}

int Klower (MonsID player)
{
	//if (player->zloc > 0)
	//	mons_move (player, player->dlevel, player->zloc - 1, player->yloc, player->xloc);
	return 0;
}

int Kthrow_cand (MonsID player)
{
	return 1;
}

int Kthrow (MonsID player)
{
	ItemID throw = player_use_pack (player, "Throw what?", ITCAT_ALL);
	if (!throw)
		return 0;

	if (it_worn (throw))
	{
		p_msg ("You're wearing that!");
		return 0;
	}

	int zloc, yloc, xloc;
	if (!p_mvchoose (player, &zloc, &yloc, &xloc, "Throw where?", NULL, &show_path_on_overlay))
		return 0;

	ev_queue (mons_speed (player)/2, mthrow, player, throw, yloc, xloc);
	return 1;
}

int Kinv (MonsID player)
{
	show_contents (player, ITCAT_ALL, "Inventory");
	return 0;
}

int nlook_msg (struct String *str, MonsID player)
{
	int k = 0;
	int n = mons_index (player);
	struct DLevel *lvl = mons_dlv (player);
	V_ItemID itemIDs = lvl->itemIDs[n];
	str_catf (str, "\n");

	int i;
	for (i = 0; i < itemIDs->len; ++ i)
	{
		ItemID it = itemIDs->data[i];
		it_desc (it_descstr, it, 0);
		str_catf (str, "%s\n", it_descstr);
		++ k;
	}

	if (k == 0)
		str_catf (str, "You see nothing here.\n");

	return k;
}

static struct String *look_str = NULL;
void nlook_auto (MonsID player)
{
	nlook_msg (look_str, player);
	p_notify (str_data (look_str));
	str_empty (look_str);
}

int Knlook (MonsID player)
{
	nlook_msg (look_str, player);
	p_flines (str_data (look_str));
	str_empty (look_str);
	return 0;
}

//static Graph info = NULL;
void flook_callback (enum P_MV action, int dlevel, int fz, int fy, int fx, int tz, int ty, int tx)
{
	/*if (action == P_MV_END)
	{
		gra_free (info);
		return;
	}
	else if (action == P_MV_CHOOSING)
		gra_free (info);
	info = gra_init (5, 5, ty + 1 - map_graph->cy, tx - 2 - map_graph->cx, 5, 5);
	//int n = map_graph (ty, tx);
	//glyph gl = map_graph->data[n];
	//p_notify ("You see here: #g%s", gl_format (gl));
	//p_notify ("%d", cur_dlevel->num_fires[n]);
	gra_box (info, 0, 0, 4, 4);
	gra_mvaddch (info, 0, 2, ACS_BTEE);*/
}

int Kflook (MonsID player)
{
	int z, y, x;
	p_mvchoose (player, &z, &y, &x, "What are you looking for?", NULL, flook_callback);
	p_endnotify ();
	return 0;
}

//static Graph overlay = NULL;
int Kscan (MonsID player)
{
/*	if (!overlay)
		overlay = gra_init (map_graph->h, map_graph->w, 0, 0, map_graph->vh, map_graph->vw);
	int Z = player->zloc, Y = player->yloc, X = player->xloc;
	int y, x;
	struct DLevel *lvl = cur_dlevel;
	for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
	{
		if (y == 0 && x == 0)
			continue;
		int n = dlv_index (lvl, Z, Y+y, X+x);
		if (n == -1)
			continue;
		MonsID ID = lvl->monsIDs[n];
		if (!ID)
			continue;
		MonsID mons = mons_at (ID);
		struct MStatus *s = &mons->status;
		int b_y = Y - map_graph->cy - 2 + 4*y, b_x = X - map_graph->cx - 2 + 4*x;
		gra_fbox (overlay, b_y, b_x, 4, 4, ' ');
		gra_box_aux (overlay, b_y, b_x, 4, 4, ACS_PLUS, ACS_PLUS, ACS_PLUS, ACS_PLUS, ACS_HLINE, ACS_VLINE);
		gra_mvaddch (overlay, b_y+2, b_x+2, mons->gl);
		if (s->moving.arrival)
			gra_mvaddch (overlay, b_y+2+s->moving.ydir, b_x+2+s->moving.xdir, 'M'|COL_TXT(0,15,0));
		else if (s->attacking.arrival)
			gra_mvaddch (overlay, b_y+2+s->attacking.ydir, b_x+2+s->attacking.xdir, 'A'|COL_TXT(15,0,0));
	}
	gr_getch ();
	gra_clear (overlay);
	return 0;*/
	return 0;
}

/*int Kopen (MonsID player)
{
	//int y, x;
	return 0;
}

int Kclose (MonsID player)
{
	//int y, x;
	return 0;
}*/

int Kwield_cand (MonsID player)
{
	return !mons_charging (player);
}

int Kwield (MonsID player)
{
	ItemID wieldID = show_contents (player, (1<<ITCAT_WEAPON) | (1<<ITCAT_HANDS), "Wield what?");
	if (wieldID == -1)
		return 0;
	if (wieldID == mons_getweap (player, 0))
		return 0;

	ev_queue (mons_speed (player)/2, mwield, player, 0, wieldID);
	return 1;
}

int Kwear_cand (MonsID player)
{
	return !mons_charging (player);
}

int Kwear (MonsID player)
{
	ItemID wear = player_use_pack (player, "Wear what?", 1<<ITCAT_ARMOUR);
	if (!wear)
		return 0;
	return mons_try_wear (player, wear);
}

int Ktakeoff_cand (MonsID player)
{
	return !mons_charging (player);
}

int Ktakeoff (MonsID player)
{
	ItemID item = player_use_pack (player, "Take off what?", 1<<ITCAT_ARMOUR);
	if (!item)
	{
		p_msg ("Not an item.");
		return 0;
	}
	if (!it_worn (item))
	{
		p_msg ("You're not wearing that!");
		return 0;
	}
	return mons_try_takeoff (player, item);
}

/*
int Klookdn (MonsID player)
{
	if (cur_dlevel->dnlevel != 0)
		dlv_set (cur_dlevel->dnlevel);
	return 0;
}

int Klookup (MonsID player)
{
	if (cur_dlevel->uplevel != 0)
		dlv_set (cur_dlevel->uplevel);
	return 0;
}*/
/*
int Kgodown (MonsID player)
{
	int level = cur_dlevel->dnlevel;
	if (level == 0)
		return 0;

	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}

int Kgoup (MonsID player)
{
	int level = cur_dlevel->uplevel;
	if (level == 0)
		return 0;

	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}*/

/*int Ksave (MonsID player)
{
	U.playing = PLAYER_SAVEGAME;
	return -1;
}*/

int Kquit (MonsID player)
{
	if (!quit())
	{
		U.playing = PLAYER_LOSTGAME;
		return -1;
	}
	return 0;
}

int Kdebug (MonsID player)
{
	return 0;
}

int p_move (int *ymove, int *xmove, char key)
{
	switch (key)
	{
		case 'k':
			*ymove = -1;
			*xmove =  0;
			return 1;
		case 'j':
			*ymove =  1;
			*xmove =  0;
			return 1;
		case 'h':
			*ymove =  0;
			*xmove = -1;
			return 1;
		case 'l':
			*ymove =  0;
			*xmove =  1;
			return 1;
		case 'y':
			*xmove = -1;
			*ymove = -1;
			return 1;
		case 'u':
			*xmove =  1;
			*ymove = -1;
			return 1;
		case 'b':
			*xmove = -1;
			*ymove =  1;
			return 1;
		case 'n':
			*xmove =  1;
			*ymove =  1;
			return 1;
		case '.':
			*xmove =  0;
			*ymove =  0;
			return 1;
		default:
			*xmove =  0;
			*ymove =  0;
	}
	return 0;
}

struct KStruct Keys[] = {
	{GRK_UP, &Kcamup,  NULL},
	{GRK_DN, &Kcamdn,  NULL},
	{GRK_LT, &Kcamlt,  NULL},
	{GRK_RT, &Kcamrt,  NULL},
	{GRK_ESC,&Kstatus, NULL},
	{' ',    &Kstatus, NULL},
	{'s',    &Kskills, NULL},
	{'.',    &Kwait,   &Kwait_cand},
	{',',    &Kpickup, &Kpickup_cand},
	{'e',    &Kevade,  NULL},
//	{'p', &Kparry},
	{'p',    &Kshield, &Kshield_cand},
	{'d',    &Ksdrop,  &Ksdrop_cand},
	{'D',    &Kmdrop,  &Kmdrop_cand},
	{'F',    &Kfmove,  &Kfmove_cand},
	{'K',    &Krise,   NULL},
	{'J',    &Klower,  NULL},
	{'m',    &Kgmove,  &Kgmove_cand},
	{'t',    &Kthrow,  &Kthrow_cand},
	{'i',    &Kinv,    NULL},
	{':',    &Knlook,  NULL},
	{';',    &Kflook,  NULL},
	{'/',    &Kscan,   NULL},
	{'Z',    &Kdebug,  NULL},
//	{'o', &Kopen},
//	{'c', &Kclose},
	{'w',    &Kwield,  &Kwield_cand},
	{'W',    &Kwear,   &Kwear_cand},
	{'T',    &Ktakeoff,&Ktakeoff_cand},
//	{CONTROL_(GRK_DN), &Klookdn},
//	{CONTROL_(GRK_UP), &Klookup},
//	{'>',    &Kgodown},
//	{'<',    &Kgoup},
//	{'S',    &Ksave,   NULL},
	{GR_CTRL('q'), &Kquit, NULL}
};

int key_lookup (MonsID player, char ch)
{
	int i;
	for (i = 0; i < NUM_KEYS; ++ i)
	{
		if (ch == Keys[i].key)
		{
			if (Keys[i].cand == NULL || (*Keys[i].cand) (player))
				return (*Keys[i].action) (player);
			return 0;
		}
	}
	return 0;
}

void pl_init ()
{
	look_str = str_dinit ();
}

int pl_charge_action (MonsID player)
{
	return -1;
}

void pl_poll (MonsID player)
{
	//if (gra_nearedge (map_graph, player->yloc, player->xloc))
	//	gra_centcam (map_graph, player->yloc, player->xloc);
	//extern Graph map_graph;
	//printf("%d %d %d   ", map_graph->cz, map_graph->cy, map_graph->cx);
	while (1)
	{
		int dlevel, z, y, x;
		mons_getloc (player, &dlevel, &z, &y, &x);
		grx_movecam (map_graph, z-3, -map_graph->gldy * z, -map_graph->gldx * z, 0);
		nlook_auto (player);
		grx_cmove (map_graph, z, y, x);

		char in = p_getch (player);

		int ymove, xmove;
		p_move (&ymove, &xmove, in);
		if (ymove != 0 || xmove != 0)
		{
			int mv = pl_attempt_move (player, ymove, xmove);
			if (mv)
				break;
	//		if (gra_nearedge (map_graph, player->yloc, player->xloc))
	//			gra_centcam (map_graph, player->yloc, player->xloc);
		}
		else
		{
			int res = key_lookup (player, in);
			if (res == 1)
				break;
			else if (res == 0)
				continue;
			else
				return;
		}
	}
	return;
}

/* returns whether the move was used up */
int pl_attempt_move (MonsID pl, int y, int x) /* each either -1, 0 or 1 */
{
	struct DLevel *lvl = mons_dlv (pl);
	int dlevel, zloc, yloc, xloc;
	mons_getloc (pl, &dlevel, &zloc, &yloc, &xloc);
	yloc += y;
	xloc += x;
	if (yloc < 0 || yloc >= lvl->h ||
	    xloc < 0 || xloc >= lvl->w)
		return 0;
	int n = dlv_index (lvl, zloc, yloc, xloc);
	/* melee attack! */
	if (lvl->monsIDs[n])
	{
		MonsID mons = lvl->monsIDs[n];
		CTR_MODE m = mons_ctrl (mons);
		if (m == CTR_AI_HOSTILE || m == CTR_AI_AGGRO)
		{
			mons_try_hitm (pl, y, x);
			return 1;
		}
		p_msg ("It says hi!");
		return 0;
	}
	/* you can and everything's fine, nothing doing */
	if (mons_can_move (pl, 0, y, x))
	{
		mons_try_move (pl, 0, y, x);
		return 1;
	}
	else if (mons_can_move (pl, -1, y, x))
	{
		mons_try_move (pl, -1, y, x);
		return 1;
	}
	else if (mons_can_move (pl, 1, y, x))
	{
		mons_try_move (pl, 1, y, x);
		return 1;
	}
	return 0;
}


void ask_items (const MonsID player, V_ItemID it_out, V_ItemID it_in, const char *msg)
{
	if (it_in->len == 1)
	{
		v_push (it_out, &it_in->data[0]);
		return;
	}
	int i;
	struct String *fmt = str_dinit ();
	str_catf (fmt, "%s\n\n", msg);
	for (i = 0; i < it_in->len; ++ i)
	{
		ItemID item = it_in->data[i];
		it_desc (it_descstr, item, 0);
		str_catf (fmt, "#o%s\n", it_descstr);
	}
	str_catf (fmt, "\n");
	int a = p_flines (str_data (fmt));
	str_free (fmt);
	if (a != -1)
		v_push (it_out, &it_in->data[a]);
}

void pl_choose_attr_gain (MonsID player, int points)
{
	int choice = p_flines (
	"#cYou gain a level! +1 to all attributes.\n"
	"#cSelect another attribute to increase by one.\n"
	"#o#g"FMT_STR" Strength\n"
	"#o#g"FMT_CON" Constitution\n"
	"#o#g"FMT_WIS" Wisdom\n"
	"#o#g"FMT_AGI" Agility");
	switch (choice)
	{
	/*case 0:
		++ player->str;
		return;
	case 1:
		++ player->con;
		return;
	case 2:
		++ player->wis;
		return;
	case 3:
		++ player->agi;
		return;*/
	default:
		return;
	}
}

ItemID player_use_pack (MonsID th, char *msg, uint32_t accepted)
{
	return show_contents (th, accepted, msg);
}

