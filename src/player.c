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

int Kwait (struct Monster *player)
{
	//mons_usedturn (player);
	return pl_execute (player->speed, player, 1);
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
		mons_usedturn (player);
		/* One item on ground -- pick up immediately. */
		pickup = ground;
	}
	else if (ground->len > 1)
	{
		mons_usedturn (player); // TODO
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
	mons_usedturn (player);
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

int Ksdrop (struct Monster *player)
{
	struct Item *drop = player_use_pack (player, "Drop what?", ITCAT_ALL);
	if (drop == NULL)
		return 0;
	mons_usedturn (player);

	Vector vdrop = v_dinit (sizeof(struct Item *));
	v_push (vdrop, &drop);

	if (drop->attr & ITEM_WIELDED)
		pl_queue (player, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, &no_item}});
	pl_queue (player, (union Event) { .mdrop = {EV_MDROP, player->ID, vdrop}});
	return pl_execute (player->speed, player, 0);
}

int Kmdrop (struct Monster *player)
{
	return 0;
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

int Kwield (struct Monster *player)
{
	struct Item *wield = player_use_pack (player, "Wield what?", ITCAT_ALL);
	if (wield == NULL)
		return 0;
	if (items_equal (wield, player->wearing.weaps[0]))
		return 0;

	mons_usedturn (player);
	if (player->wearing.weaps[0] && wield->type.type)
		pl_queue (player, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, &no_item}});
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

	mons_usedturn (player);
	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}

int Kgoup (struct Monster *player)
{
	int level = cur_dlevel->uplevel;
	if (level == 0)
		return 0;

	mons_usedturn (player);
	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}*/

int Ksave (struct Monster *player)
{
	U.playing = PLAYER_SAVEGAME;
	return -1;
}

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

struct KStruct Keys[] = {
	{GRK_UP, &Kcamup},
	{GRK_DN, &Kcamdn},
	{GRK_LF, &Kcamlf},
	{GRK_RT, &Kcamrt},
	{CH_ESC, &Kstatus},
	{'s', &Kskills},
	{'.', &Kwait},
	{',', &Kpickup},
//	{'e', &Keat},
//	{'e', &Kevade},
//	{'p', &Kparry},
	{'p', &Kshield},
	{'d', &Ksdrop},
	{'D', &Kmdrop},
	{'i', &Kinv},
	{':', &Knlook},
	{';', &Kflook},
	{'Z', &Kdebug},
//	{'o', &Kopen},
//	{'c', &Kclose},
	{'w', &Kwield},
//	{CONTROL_(GRK_DN), &Klookdn},
//	{CONTROL_(GRK_UP), &Klookup},
//	{'>', &Kgodown},
//	{'<', &Kgoup},
	{'S', &Ksave},
	{CONTROL_('q'), &Kquit}
};

int key_lookup (struct Monster *player, char key)
{
	int i;
	char ch = (char) key;
	for (i = 0; i < NUM_KEYS; ++ i)
	{
		//if (ch == (char)(Keys[i].key&0xff) && gr_equiv (key, Keys[i].key))
		if (ch == Keys[i].key)
			return (*Keys[i].action) (player);
	}
	return 0;
}

int player_move_gently (struct Monster *player)
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

int player_move_attack (struct Monster *player)
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

int pl_take_turn (struct Monster *player)
{
	if (gra_nearedge (map_graph, player->yloc, player->xloc))
		gra_centcam (map_graph, player->yloc, player->xloc);
	while (1)
	{
		char in;
	
		draw_map (player);
		p_pane (player);

		gra_cmove (map_graph, player->yloc, player->xloc);

		in = gr_getch();
		if (in == 'm')
		{
			if (player_move_gently (player))
				break;
			continue;	
		}
		else if (in == 'F')
		{
			if (player_move_attack (player))
				break;
			continue;
		}
		else if (in == 'f')
		{
			pl_focus_mode = !pl_focus_mode;
			continue;
		}

		int ymove, xmove;
		p_move (&ymove, &xmove, in);
		if (!(ymove == 0 && xmove == 0))
		{
			int mv = mons_move (player, ymove, xmove, 1);
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

