/* player.c */

#include "include/player.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/save.h"
#include "include/item.h"

#include <malloc.h>

int Kcamup ()
{
	gra_movecam (map_graph, map_graph->cy - 10, map_graph->cx);
	return 0;
}

int Kcamdn ()
{
	gra_movecam (map_graph, map_graph->cy + 10, map_graph->cx);
	return 0;
}

int Kcamlf ()
{
	gra_movecam (map_graph, map_graph->cy, map_graph->cx - 10);
	return 0;
}

int Kcamrt ()
{
	gra_movecam (map_graph, map_graph->cy, map_graph->cx + 10);
	return 0;
}

int Kwait ()
{
	mons_usedturn (player);
	return 1;
}

int Kpickup ()
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

	if (ground->len == 1)
	{
		mons_usedturn (player);
		/* One item on ground -- pick up immediately. */
		if (pack_add (&pmons.pack, &THIID(*(int*)v_at (ground, 0))->thing.item))
			rem_id (*(int*)v_at (ground, 0));
	
		v_free (ground);
	}
	else if (ground->len > 1)
	{
		mons_usedturn (player); // TODO
		/* Multiple items - ask which to pick up. */
		Vector pickup = v_init (sizeof(int), 20);
	
		/* Do the asking */
		ask_items (pickup, ground, "Pick up what?");
		v_free (ground);
	
		/* Put items in ret_list into inventory. The loop
		* continues until ret_list is done or the pack is full. */
		for (i = 0; i < pickup->len; ++ i)
		{
			/* Pick up the item; quit if the bag is full */
			th = THIID(*(int*)v_at (pickup, i));
			if (!pack_add (&pmons.pack, &th->thing.item))
				break;
			/* Remove item from main play */
			rem_id (th->ID);
		}
		v_free (pickup);
	}
	return 1;
}

int Keat ()
{
	struct Item *food = player_use_pack ("Eat what?", ITCAT_FOOD);
	if (food == NULL)
		return 0;
	mons_usedturn (player);
	mons_eat (player, food);
	return 1;
}

int Ksdrop ()
{
	struct Item *drop = player_use_pack ("Drop what?", ITCAT_ALL);
	if (drop == NULL)
		return 0;
	mons_usedturn (player);
	if (drop == player->thing.mons.wearing.arms)
		mons_unwield (player);
	unsigned u = PACK_AT (get_Itref (pmons.pack, drop));
	pmons.pack.items[u] = NULL;
	new_thing (THING_ITEM, cur_dlevel, player->yloc, player->xloc, drop);
	return 1;
}

int Kmdrop ()
{
	return 0;
}

int Kinv ()
{
	show_contents (pmons.pack, ITCAT_ALL, "Inventory");
	return 0;
}

int Knlook ()
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

		char *line = get_inv_line (&THING(things, n, i)->thing.item);
		char out[256];
		sprintf (out, "You%s see here %s.", (k ? " also" : ""), line);
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

int Kflook ()
{
	return 0;
}

int Kwield ()
{
	struct Item *wield = player_use_pack ("Wield what?", ITCAT_ALL);
	if (wield == NULL)
		return 0;

	mons_usedturn (player);
	if (mons_unwield (player))
		mons_wield (player, wield);

	return 1;
}

int Klookdn ()
{
	if (cur_dlevel->dnlevel != 0)
		dlv_set (cur_dlevel->dnlevel);
	return 0;
}

int Klookup ()
{
	if (cur_dlevel->uplevel != 0)
		dlv_set (cur_dlevel->uplevel);
	return 0;
}

int Kgodown ()
{
	int level = cur_dlevel->dnlevel;
	if (level == 0)
		return 0;

	mons_usedturn (player);
	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}

int Kgoup ()
{
	int level = cur_dlevel->uplevel;
	if (level == 0)
		return 0;

	mons_usedturn (player);
	dlv_set (level);
	thing_move (player, level, player->yloc, player->xloc);
	return 1;
}

int Ksave ()
{
	U.playing = PLAYER_SAVEGAME;
	return -1;
}

int Kquit ()
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
	{'.', &Kwait},
	{',', &Kpickup},
	{'e', &Keat},
	{'d', &Ksdrop},
	{'D', &Kmdrop},
	{'i', &Kinv},
	{':', &Knlook},
	{';', &Kflook},
	{'w', &Kwield},
	{CONTROL_(GRK_DN), &Klookdn},
	{CONTROL_(GRK_UP), &Klookup},
	{'>', &Kgodown},
	{'<', &Kgoup},
	{'S', &Ksave},
	{CONTROL_('q'), &Kquit}
};

int key_lookup (uint32_t key)
{
	int i;
	char ch = (char) key;
	for (i = 0; i < NUM_KEYS; ++ i)
	{
		if (ch == (char)(Keys[i].key&0xff) && gr_equiv (key, Keys[i].key))
			return (*Keys[i].action) ();
	}
	return 0;
}

uint32_t pl_move (int *ymove, int *xmove, uint32_t key)
{
	switch (key)
	{
		case 'k':
			*ymove = -1;
			*xmove =  0;
			break;
		case 'j':
			*ymove =  1;
			*xmove =  0;
			break;
		case 'h':
			*ymove =  0;
			*xmove = -1;
			break;
		case 'l':
			*ymove =  0;
			*xmove =  1;
			break;
		case 'y':
			*xmove = -1;
			*ymove = -1;
			break;
		case 'u':
			*xmove =  1;
			*ymove = -1;
			break;
		case 'b':
			*xmove = -1;
			*ymove =  1;
			break;
		case 'n':
			*xmove =  1;
			*ymove =  1;
			break;
		default:
			*xmove =  0;
			*ymove =  0;
	}
	return key;
}

void pl_mvchoose (int *yloc, int *xloc, char *instruct, char *confirm)
{
	/*if (instruct)
		p_msg (instruct);
	p_pane ();
	gr_refresh ();
	int xmove, ymove;
	uint32_t key = pl_move (&ymove, &xmove, gr_getfullch ());
	while (key != '.' || (confirm && (p_ask ("yn", confirm) != 'y')))
	{
		if (ymove == -1 && csr_y > 0)
			csr_move (csr_y-1, csr_x);
		else if (ymove == 1 && csr_y < map_graph->vh-1)
			csr_move (csr_y+1, csr_x);
		if (xmove == -1 && csr_x > 0)
			csr_move (csr_y, csr_x-1);
		else if (xmove == 1 && csr_x < map_graph->vw-1)
			csr_move (csr_y, csr_x+1);
		if (gra_nearedge (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x))
			gra_centcam (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x);
		key = pl_move (&ymove, &xmove, gr_getfullch ());
	}
	*yloc = map_graph->cy + csr_y;
	*xloc = map_graph->cx + csr_x;*/
}

