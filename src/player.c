/* player.c */

#include "include/player.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/save.h"
#include "include/item.h"
#include "include/event.h"

int pl_focus_mode = 0;
Vector player_actions = NULL;
void pl_queue (struct Monster *player, union Event ev)
{
	struct QEv qev = {ev_delay (&ev), ev};
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
	ev_queue (wait+1, (union Event) {.mturn = {EV_MTURN, player->ID}});
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
	pl_queue (player, (union Event) { .mstopcharge = {EV_MSTOPCHARGE, player->ID}});
	return pl_execute (player->speed, player, 1);
}

int Kpickup_cand (struct Monster *player)
{
	return player->status.charging == 0;
}

int Kpickup (struct Monster *player)
{
	Vector ground = v_init (sizeof (int), 20);
	int n = map_buffer (player->yloc, player->xloc);
	Vector *things = dlv_things (player->dlevel);
	struct Thing *th;

	LOOP_THING(things, n, i)
	{
		th = THING(things, n, i);
		if (th->type == THING_ITEM)
			v_push (ground, &th->ID);
	}

	if (ground->len < 1)
		return 0;

	Vector pickup = NULL;
	if (ground->len == 1)
	{
		/* One item on ground -- pick up immediately. */
		pickup = ground;
	}
	else if (ground->len > 1)
	{
		/* Multiple items - ask which to pick up. */
		pickup = v_init (sizeof(TID), 20);

		/* Do the asking */
		ask_items (pickup, ground, "Pick up what?");
		v_free (ground);

	}
	pl_queue (player, (union Event) { .mpickup = {EV_MPICKUP, player->ID, pickup}});
	return pl_execute (player->speed, player, 0);
}

/*int Keat (struct Monster *player)
{
	struct Item *food = player_use_pack ("Eat what?", ITCAT_FOOD);
	if (food == NULL)
		return 0;
	mons_eat (player, food);
	return 1;
}*/

/*int Kevade (struct Monster *player)
{
	pl_queue (player, (union Event) { .mevade = {EV_MEVADE, player->ID}});
	return pl_execute (player->speed/2, player, 0);
}*/
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
	p_msg ("Shield in which direction?");
	p_pane (player);
	char in = gr_getch ();
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
	if (drop == NULL)
		return 0;

	Vector vdrop = v_dinit (sizeof(TID));
	v_push (vdrop, &drop);

	if (drop->attr & ITEM_WIELDED)
		pl_queue (player, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, &no_item}});
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
	char in = gr_getch ();
	if (in == CH_ESC)
		return 0;
	
	int ymove, xmove;
	if (!p_move (&ymove, &xmove, in))
	{
		p_msg ("That's not a direction!");
		return 0;
	}
	pl_queue (player, (union Event) { .mattkm = {EV_MATTKM, player->ID, ymove, xmove}});
	return pl_execute (player->speed, player, 0);
}

int Kgmove_cand (struct Monster *player)
{
	return 1;
}

int Kgmove (struct Monster *player)
{
	char in = gr_getch ();
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

int Kinv (struct Monster *player)
{
	show_contents (player->pack, ITCAT_ALL, "Inventory");
	return 0;
}

int Knlook (struct Monster *player)
{
	int k = 0;
	int n = map_buffer (player->yloc, player->xloc);
	Vector *things = cur_dlevel->things;
	Vector list = v_dinit (256);

	LOOP_THING(things, n, i)
	{
		struct Thing *th = THING(things, n, i);
		if (th->type != THING_ITEM)
			continue;

		char *line = get_inv_line (NULL, &THING(things, n, i)->thing.item);
		char out[256];
		snprintf (out, 256, "You%s see here %s.", (k ? " also" : ""), line);
		v_pstr (list, out);
		++ k;
		free (line);
	}
	if (k == 0)
		p_msg ("You see nothing here. ");
	else
		p_lines (list);

	v_free (list);

	return 0;
}

int Kflook (struct Monster *player)
{
	int y, x;
	p_mvchoose (player, &y, &x, "What are you looking for?", NULL, 0);
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
	struct Item *wield = player_use_pack (player, "Wield what?", ITCAT_ALL);
	if (wield == NULL)
		return 0;
	if (items_equal (wield, player->wearing.weaps[0]))
		return 0;

	if (NO_ITEM(wield))
		wield = NULL;
	if (player->wearing.weaps[0] && wield)
		pl_queue (player, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, NULL}});
	pl_queue (player, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, wield}});
	return pl_execute (player->speed, player, 0);
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
//	{'e', &Kevade},
//	{'p', &Kparry},
	{'p',    &Kshield, &Kshield_cand},
	{'d',    &Ksdrop,  &Ksdrop_cand},
	{'D',    &Kmdrop,  &Kmdrop_cand},
	{'F',    &Kfmove,  &Kfmove_cand},
	{'m',    &Kgmove,  &Kgmove_cand},
	{'i',    &Kinv,    NULL},
	{':',    &Knlook,  NULL},
	{';',    &Kflook,  NULL},
	{'Z',    &Kdebug,  NULL},
	{'f',    &Kfocus,  &Kfocus_cand},
//	{'o', &Kopen},
//	{'c', &Kclose},
	{'w',    &Kwield,  &Kwield_cand},
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

int pl_charge_action (struct Monster *player)
{
	return -1;
}

int pl_take_turn (struct Monster *player)
{
	if (gra_nearedge (map_graph, player->yloc, player->xloc))
		gra_centcam (map_graph, player->yloc, player->xloc);
	while (1)
	{
		draw_map (player);
		p_pane (player);

		gra_cmove (map_graph, player->yloc, player->xloc);

		char in = gr_getch();

		int ymove, xmove;
		p_move (&ymove, &xmove, in);
		if (ymove != 0 || xmove != 0)
		{
			int mv = mons_move (player, ymove, xmove);
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

