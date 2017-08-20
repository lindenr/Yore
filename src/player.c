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

int Kcamup (struct Thing *player)
{
	gra_movecam (map_graph, map_graph->cy - 10, map_graph->cx);
	return 0;
}

int Kcamdn (struct Thing *player)
{
	gra_movecam (map_graph, map_graph->cy + 10, map_graph->cx);
	return 0;
}

int Kcamlf (struct Thing *player)
{
	gra_movecam (map_graph, map_graph->cy, map_graph->cx - 10);
	return 0;
}

int Kcamrt (struct Thing *player)
{
	gra_movecam (map_graph, map_graph->cy, map_graph->cx + 10);
	return 0;
}

/*int Kstatus (struct Thing *player)
{
	return p_status (P_STATUS);
}

int Kskills (struct Thing *player)
{
	return p_status (P_SKILLS);
}*/

int Kwait (struct Thing *player)
{
	//mons_usedturn (player);
	ev_queue (player->thing.mons.speed, (union Event) { .mturn = {EV_MTURN, player->ID}});
	return 1;
}

int Kpickup (struct Thing *player)
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

	Vector pickup;
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
	ev_queue (0, (union Event) { .mpickup = {EV_MPICKUP, player->ID, pickup}});
	return 1;
}

/*int Keat (struct Thing *player)
{
	struct Item *food = player_use_pack ("Eat what?", ITCAT_FOOD);
	if (food == NULL)
		return 0;
	mons_usedturn (player);
	mons_eat (player, food);
	return 1;
}*/

int Ksdrop (struct Thing *player)
{
	struct Item *drop = player_use_pack (player, "Drop what?", ITCAT_ALL);
	if (drop == NULL)
		return 0;
	mons_usedturn (player);

	Vector vdrop = v_dinit (sizeof(struct Item *));
	v_push (vdrop, &drop);

	ev_queue (0, (union Event) { .mdrop = {EV_MDROP, player->ID, vdrop}});
	return 1;
}

int Kmdrop (struct Thing *player)
{
	return 0;
}

int Kinv (struct Thing *player)
{
	show_contents (player->thing.mons.pack, ITCAT_ALL, "Inventory");
	return 0;
}

int Knlook (struct Thing *player)
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

int Kflook (struct Thing *player)
{
	int y, x; 
	p_mvchoose (player, &y, &x, "What are you looking for?", NULL, 0);
	return 0;
}
/*
int Kwield (struct Thing *player)
{
	struct Item *wield = player_use_pack ("Wield what?", ITCAT_ALL);
	if (wield == NULL)
		return 0;

	mons_usedturn (player);
	if (mons_unwield (player))
		mons_wield (player, wield);

	return 1;
}*/
/*
int Klookdn (struct Thing *player)
{
	if (cur_dlevel->dnlevel != 0)
		dlv_set (cur_dlevel->dnlevel);
	return 0;
}

int Klookup (struct Thing *player)
{
	if (cur_dlevel->uplevel != 0)
		dlv_set (cur_dlevel->uplevel);
	return 0;
}*/
/*
int Kgodown (struct Thing *player)
{
	int level = cur_dlevel->dnlevel;
	if (level == 0)
		return 0;

	mons_usedturn (player);
	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}

int Kgoup (struct Thing *player)
{
	int level = cur_dlevel->uplevel;
	if (level == 0)
		return 0;

	mons_usedturn (player);
	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}*/

int Ksave (struct Thing *player)
{
	U.playing = PLAYER_SAVEGAME;
	return -1;
}

int Kquit (struct Thing *player)
{
	if (!quit())
	{
		U.playing = PLAYER_LOSTGAME;
		return -1;
	}
	return 0;
}

struct KStruct Keys[] = {
	{GRK_UP, &Kcamup},
	{GRK_DN, &Kcamdn},
	{GRK_LF, &Kcamlf},
	{GRK_RT, &Kcamrt},
//	{CH_ESC, &Kstatus},
//	{'s', &Kskills},
	{'.', &Kwait},
	{',', &Kpickup},
//	{'e', &Keat},
	{'d', &Ksdrop},
	{'D', &Kmdrop},
	{'i', &Kinv},
	{':', &Knlook},
	{';', &Kflook},
//	{'w', &Kwield},
//	{CONTROL_(GRK_DN), &Klookdn},
//	{CONTROL_(GRK_UP), &Klookup},
//	{'>', &Kgodown},
//	{'<', &Kgoup},
	{'S', &Ksave},
	{CONTROL_('q'), &Kquit}
};

int key_lookup (struct Thing *player, uint32_t key)
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

