/* player.c */

#include "include/player.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/save.h"
#include "include/item.h"
//#include "include/skills.h"
#include "include/event.h"

int cur_players;

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
	ev_queue (player->speed, (union Event) { .mturn = {EV_MTURN, player->ID}});
	return 1;
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
	ev_queue (player->speed, (union Event) { .mpickup = {EV_MPICKUP, player->ID, pickup}});
	return 1;
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

int Kevade (struct Monster *player)
{
	ev_queue (0, (union Event) { .mevade = {EV_MEVADE, player->ID}});
	return 1;
}

int player_try_drop (struct Monster *player)
{
	// TODO check can be dropped?
	ev_queue (0, (union Event) { .mdrop = {EV_MDROP, player->ID, player->ai.cont.data}});
	return 0;
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
	{
		ev_queue (player->speed, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, &no_item}});
		mons_cont (player, &player_try_drop, vdrop, sizeof(*vdrop));
		return 1;
	}
	ev_queue (player->speed, (union Event) { .mdrop = {EV_MDROP, player->ID, vdrop}});
	return 1;
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

int player_try_wield (struct Monster *player)
{
	ev_queue (0, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, player->ai.cont.data}});
	return 0;
}

int Kwield (struct Monster *player)
{
	struct Item *wield = player_use_pack (player, "Wield what?", ITCAT_ALL);
	if (wield == NULL)
		return 0;
	if (items_equal (wield, player->wearing.weaps[0]))
		return 0;

	mons_usedturn (player);
	if (player->wearing.weaps[0] && wield->type.type)
	{
		ev_queue (player->speed, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, &no_item}});
		mons_cont (player, &player_try_wield, wield, sizeof (*wield));
		return 1;
	}
	ev_queue (player->speed, (union Event) { .mwield = {EV_MWIELD, player->ID, 0, wield}});
	return 1;
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
	{'e', &Kevade},
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

int key_lookup (struct Monster *player, uint32_t key)
{
	int i;
	char ch = (char) key;
	for (i = 0; i < NUM_KEYS; ++ i)
	{
		if (ch == (char)(Keys[i].key&0xff) && gr_equiv (key, Keys[i].key))
			return (*Keys[i].action) (player);
	}
	return 0;
}
