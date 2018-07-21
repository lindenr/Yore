/* player.c */

#include "include/player.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/save.h"
#include "include/item.h"
#include "include/event.h"
#include "include/string.h"

static char it_descstr[IT_DESC_LEN];

int pl_focus_mode = 0;
Vector player_actions = NULL;
void pl_queue (struct Monster *player, union Event ev)
{
	struct QEv qev = {ev_delay (&ev), 0, ev};
	v_push (player_actions, &qev);
}

int pl_execute (Tick wait, struct Monster *player, int force)
{
	if (force == 0 && pl_focus_mode)
		return 0;
	int i;
	for (i = 0; i < player_actions->len; ++ i)
	{
		struct QEv *qev = v_at (player_actions, i);
		ev_queue (qev->tick, qev->ev);
	}
	player_actions->len = 0;
	//ev_queue (wait+1, (union Event) {.mturn = {EV_MTURN, player->ID}});
	return 1;
}

int Kcamup (struct Monster *player)
{
	gra_movecam (map_graph, map_graph->cy - 10, map_graph->cx);
	return 0;
}

int Kcamdn (struct Monster *player)
{
	gra_movecam (map_graph, map_graph->cy + 10, map_graph->cx);
	return 0;
}

int Kcamlf (struct Monster *player)
{
	gra_movecam (map_graph, map_graph->cy, map_graph->cx - 10);
	return 0;
}

int Kcamrt (struct Monster *player)
{
	gra_movecam (map_graph, map_graph->cy, map_graph->cx + 10);
	return 0;
}

int Kstatus (struct Monster *player)
{
	return p_status (player, P_STATUS);
}

int Kskills (struct Monster *player)
{
	return p_status (player, P_SKILLS);
}

int Kwait_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kwait (struct Monster *player)
{
	//pl_queue (player, (union Event) { .mstopcharge = {EV_MSTOPCHARGE, player->ID}});
	//return pl_execute (player->speed/5, player, 1);
	ev_queue (player->speed/5, (union Event) { .mturn = {EV_MTURN, player->ID}});
	return 1;
}

int Kpickup_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kpickup (struct Monster *player)
{
	int n = map_buffer (player->yloc, player->xloc);
	Vector ground = dlv_items (player->dlevel)[n];

	if (ground->len <= 0)
		return 0;

	Vector pickup = NULL;
	if (ground->len == 1)
	{
		/* One item on ground -- pick up immediately. */
		pickup = v_init (sizeof(TID), 1);
		v_push (pickup, &(((struct Item *)v_at (ground, 0))->ID));
	}
	else if (ground->len > 1)
	{
		/* Multiple items - ask which to pick up. */
		pickup = v_init (sizeof(TID), 20);

		/* Do the asking */
		ask_items (player, pickup, ground, "Pick up what?");
		if (pickup->len == 0)
			return 0;

	}
	ev_queue (player->speed, (union Event) { .mpickup = {EV_MPICKUP, player->ID, pickup}});
	//return pl_execute (player->speed, player, 0);
	return 1;
}

/*int Keat (struct Monster *player)
{
	struct Item *food = player_use_pack ("Eat what?", ITCAT_FOOD);
	if (food == NULL)
		return 0;
	mons_eat (player, food);
	return 1;
}*/

int Kevade (struct Monster *player)
{
	int ymove, xmove;
	p_notify ("Evade in which direction?");
	char in = p_getch (player);
	p_endnotify ();
	p_move (&ymove, &xmove, in);
	if (ymove == 0 && xmove == 0)
		return 0;
	pl_queue (player, (union Event) { .mevade = {EV_MEVADE, player->ID, ymove, xmove}});
	return pl_execute (player->speed/2, player, 0);
}
/*
int Kparry (struct Monster *player)
{
//	pl_queue (player, (union Event) { .mparry = {EV_MPARRY, player->ID}});
	return 0;
}*/

int Kshield_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kshield (struct Monster *player)
{
	int ymove, xmove;
	p_notify ("Shield in which direction?");
	char in = p_getch (player);
	p_endnotify ();
	p_move (&ymove, &xmove, in);
	if (ymove == 0 && xmove == 0)
		return 0;
	pl_queue (player, (union Event) { .mshield = {EV_MSHIELD, player->ID, ymove, xmove}});
	return pl_execute (player->speed, player, 0);
}

int Ksdrop_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Ksdrop (struct Monster *player)
{
	struct Item *drop = player_use_pack (player, "Drop what?", ITCAT_ALL);
	if (NO_ITEM(drop))
		return 0;

	if (it_worn (drop))
	{
		p_msg ("You're wearing that!");
		return 0;
	}

	Vector vdrop = v_dinit (sizeof(TID));
	v_push (vdrop, &drop->ID);

	pl_queue (player, (union Event) { .mdrop = {EV_MDROP, player->ID, vdrop}});
	return pl_execute (player->speed, player, 0);
}

int Kmdrop_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kmdrop (struct Monster *player)
{
	return 0;
}

int Kfmove_cand (struct Monster *player)
{
	return 1;
}

int Kfmove (struct Monster *player)
{
	char in = p_getch (player);
	if (in == CH_ESC)
		return 0;
	
	int ymove, xmove;
	if (!p_move (&ymove, &xmove, in))
	{
		p_msg ("That's not a direction!");
		return 0;
	}
	//pl_queue (player, (union Event) { .mattkm = {EV_MATTKM, player->ID, ymove, xmove}});
	//return pl_execute (player->speed, player, 0);
	return mons_try_attack (player, ymove, xmove);
}

int Kgmove_cand (struct Monster *player)
{
	return 1;
}

int Kgmove (struct Monster *player)
{
	char in = p_getch (player);
	if (in == CH_ESC)
		return 0;
	
	int ymove, xmove;
	if (!p_move (&ymove, &xmove, in))
	{
		p_msg ("That's not a direction!");
		return 0;
	}
	pl_queue (player, (union Event) { .mmove = {EV_MMOVE, player->ID, ymove, xmove}});
	return pl_execute (player->speed, player, 0);
}

int Kthrow_cand (struct Monster *player)
{
	return 1;
}

int Kthrow (struct Monster *player)
{
	struct Item *throw = player_use_pack (player, "Throw what?", ITCAT_ALL);
	if (NO_ITEM(throw))
		return 0;

	if (it_worn (throw))
	{
		p_msg ("You're wearing that!");
		return 0;
	}

	int yloc, xloc;
	p_mvchoose (player, &yloc, &xloc, "Throw where?", NULL, &show_path_on_overlay);
	if (yloc == -1)
		return 0;

	pl_queue (player, (union Event) { .mthrow = {EV_MTHROW, player->ID, throw->ID, yloc, xloc}});
	return pl_execute (player->speed, player, 0);
}

int Kinv (struct Monster *player)
{
	show_contents (player, ITCAT_ALL, "Inventory");
	return 0;
}

int nlook_msg (struct String *str, struct Monster *player)
{
	int k = 0;
	int n = map_buffer (player->yloc, player->xloc);
	struct DLevel *lvl = dlv_lvl (player->dlevel);
	Vector items = lvl->items[n];
	Vector things = lvl->things[n];
	str_catf (str, "\n");

	int i;
	for (i = 0; i < things->len; ++ i)
	{
		struct Thing *th = v_at (things, i);
		struct map_item_struct *m = &th->thing.mis;
		if (m->gl == map_items[DGN_SLIME].gl)
		{
			str_catf (str, "#g%s slime on the ground\n", gl_format (m->gl));
			++ k;
		}
		else if (m->gl == map_items[DGN_OPENDOOR].gl)
		{
			str_catf (str, "#g%s an open door\n", gl_format (m->gl));
			++ k;
		}
	}

	for (i = 0; i < items->len; ++ i)
	{
		struct Item *it = v_at(items, i);
		it_desc (it_descstr, it, NULL);
		str_catf (str, "%s\n", it_descstr);
		++ k;
	}

	if (k == 0)
		str_catf (str, "You see nothing here.\n");

	return k;
}

static struct String *look_str = NULL;
void nlook_auto (struct Monster *player)
{
	nlook_msg (look_str, player);
	p_notify (str_data (look_str));
	str_empty (look_str);
}

int Knlook (struct Monster *player)
{
	nlook_msg (look_str, player);
	p_flines (str_data (look_str));
	str_empty (look_str);
	return 0;
}

void flook_callback (enum P_MV action, int fy, int fx, int ty, int tx)
{
	int n = map_buffer (ty, tx);
	//glyph gl = map_graph->data[n];
	//p_notify ("You see here: #g%s", gl_format (gl));
	p_notify ("%d", cur_dlevel->num_fires[n]);
}

int Kflook (struct Monster *player)
{
	int y, x;
	p_mvchoose (player, &y, &x, "What are you looking for?", NULL, flook_callback);
	p_endnotify ();
	return 0;
}

/*int Kopen (struct Monster *player)
{
	//int y, x;
	return 0;
}

int Kclose (struct Monster *player)
{
	//int y, x;
	return 0;
}*/

int Kfocus_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kfocus (struct Monster *player)
{
	pl_focus_mode = !pl_focus_mode;
	return 0;
}

int Kwield_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kwield (struct Monster *player)
{
	TID wieldID = show_contents (player, (1<<ITCAT_WEAPON) | (1<<ITCAT_HANDS), "Wield what?");
	if (wieldID == -1)
		return 0;
	if (player->wearing.weaps[0] && wieldID == player->wearing.weaps[0]->ID)
		return 0;

	pl_queue (player, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, wieldID}});
	return pl_execute (player->speed, player, 0);
}

int Kwear_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kwear (struct Monster *player)
{
	struct Item *wear = player_use_pack (player, "Wear what?", 1<<ITCAT_ARMOUR);
	if (NO_ITEM(wear))
		return 0;
	return mons_try_wear (player, wear);
}

int Ktakeoff_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Ktakeoff (struct Monster *player)
{
	struct Item *item = player_use_pack (player, "Take off what?", 1<<ITCAT_ARMOUR);
	if (NO_ITEM(item))
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
int Klookdn (struct Monster *player)
{
	if (cur_dlevel->dnlevel != 0)
		dlv_set (cur_dlevel->dnlevel);
	return 0;
}

int Klookup (struct Monster *player)
{
	if (cur_dlevel->uplevel != 0)
		dlv_set (cur_dlevel->uplevel);
	return 0;
}*/
/*
int Kgodown (struct Monster *player)
{
	int level = cur_dlevel->dnlevel;
	if (level == 0)
		return 0;

	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}

int Kgoup (struct Monster *player)
{
	int level = cur_dlevel->uplevel;
	if (level == 0)
		return 0;

	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}*/

/*int Ksave (struct Monster *player)
{
	U.playing = PLAYER_SAVEGAME;
	return -1;
}*/

int Kquit (struct Monster *player)
{
	if (!quit())
	{
		U.playing = PLAYER_LOSTGAME;
		return -1;
	}
	return 0;
}

int Kdebug (struct Monster *player)
{
	ev_debug ();
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
	{GRK_LF, &Kcamlf,  NULL},
	{GRK_RT, &Kcamrt,  NULL},
	{CH_ESC, &Kstatus, NULL},
	{' ',    &Kstatus, NULL},
	{'s',    &Kskills, NULL},
	{'.',    &Kwait,   &Kwait_cand},
	{',',    &Kpickup, &Kpickup_cand},
//	{'e', &Keat},
	{'e',    &Kevade,  NULL},
//	{'p', &Kparry},
	{'p',    &Kshield, &Kshield_cand},
	{'d',    &Ksdrop,  &Ksdrop_cand},
	{'D',    &Kmdrop,  &Kmdrop_cand},
	{'F',    &Kfmove,  &Kfmove_cand},
	{'m',    &Kgmove,  &Kgmove_cand},
	{'t',    &Kthrow,  &Kthrow_cand},
	{'i',    &Kinv,    NULL},
	{':',    &Knlook,  NULL},
	{';',    &Kflook,  NULL},
	{'Z',    &Kdebug,  NULL},
//	{'f',    &Kfocus,  &Kfocus_cand},
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

int key_lookup (struct Monster *player, char ch)
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

int pl_charge_action (struct Monster *player)
{
	return -1;
}

int pl_take_turn (struct Monster *player)
{
	//draw_map ();
	//p_notify ("You see nothing here.");
	if (gra_nearedge (map_graph, player->yloc, player->xloc))
		gra_centcam (map_graph, player->yloc, player->xloc);
	while (1)
	{
		nlook_auto (player);
		gra_cmove (map_graph, player->yloc, player->xloc);

		char in = p_getch (player);

		int ymove, xmove;
		p_move (&ymove, &xmove, in);
		if (ymove != 0 || xmove != 0)
		{
			int mv = pl_attempt_move (player, ymove, xmove);
			if (mv)
				break;
			if (gra_nearedge (map_graph, player->yloc, player->xloc))
				gra_centcam (map_graph, player->yloc, player->xloc);
		}
		else
		{
			int res = key_lookup (player, in);
			if (res == 1)
				break;
			else if (res == 0)
				continue;
			else
				return 0;
		}
	}
	return 1;
}

/* returns whether the move was used up */
int pl_attempt_move (struct Monster *pl, int y, int x) /* each either -1, 0 or 1 */
{
	struct DLevel *lvl = dlv_lvl (pl->dlevel);
	int yloc = pl->yloc + y, xloc = pl->xloc + x;
	if (yloc < 0 || yloc >= map_graph->h ||
	    xloc < 0 || xloc >= map_graph->w)
		return 0;
	int i, n = map_buffer (yloc, xloc);
	/* melee attack! */
	if (lvl->monsIDs[n])
	{
		struct Monster *mons = MTHIID (lvl->monsIDs[n]);
		if (mons->ctr.mode == CTR_AI_HOSTILE ||
			mons->ctr.mode == CTR_AI_AGGRO)
		{
			mons_try_attack (pl, y, x);
			return 1;
		}
		p_msg ("It says hi!");
		return 0;
	}
	for (i = 0; i < lvl->things[n]->len; ++ i)
	{
		struct Thing *th = THING(lvl->things, n, i);
		/* like a an unmoveable boulder or something */
		if (th->type == THING_DGN && (th->thing.mis.attr & 1) == 0)
			return 0;
	}
	/* you can and everything's fine, nothing doing */
	pl_queue (pl, (union Event) { .mmove = {EV_MMOVE, pl->ID, y, x}});
	return pl_execute (pl->speed, pl, 0);
}


void ask_items (const struct Monster *player, Vector it_out, Vector it_in, const char *msg)
{
	int i;
	struct String *fmt = str_dinit ();
	str_catf (fmt, "%s\n\n", msg);
	for (i = 0; i < it_in->len; ++ i)
	{
		TID itemID = *(TID *) v_at (it_in, i);
		it_desc (it_descstr, it_at (itemID), NULL);
		str_catf (fmt, "#o%s\n", it_descstr);
	}
	str_catf (fmt, "\n");
	int a = p_flines (str_data (fmt));
	str_free (fmt);
	if (a != -1)
		v_push (it_out, v_at (it_in, a));
}

void pl_choose_attr_gain (struct Monster *player, int points)
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
	case 0:
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
		return;
	default:
		return;
	}
}

struct Item *player_use_pack (struct Monster *th, char *msg, uint32_t accepted)
{
	return it_at(show_contents (th, accepted, msg));
}

void pl_redraw ()
{
}

