/* player.c */

#include "include/player.h"
#include "include/pline.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/save.h"

struct KStruct Keys[] = {
	{',', &Kpickup},
	{'e', &Keat},
	{'d', &Ksdrop},
	{'D', &Kmdrop},
	{'i', &Kinv},
	{';', &Knlook},
	{':', &Kflook},
	{'w', &Kwield},
	{CONTROL_('s'), &Ksave},
	{CONTROL_('q'), &Kquit}
};

int Kpickup ()
{
	Vector ground = v_init (sizeof (int), 20);
	int n = to_buffer (player->yloc, player->xloc);
	
	LOOP_THING(n, i)
	{
		if (THING(n, i)->type == THING_ITEM)
			v_push (ground, &i);
	}
	
	if (ground->len < 1)
		return 0;

	if (ground->len == 1)
	{
		/* One item on ground -- pick up immediately. */
		if (pack_add (&pmons.pack, &THING(n, *(int*)v_at (ground, 0))->thing.item))
			rem_ref (n, *(int*)v_at (ground, 0));
	
		v_free (ground);
	}
	else if (ground->len > 1)
	{
		/* Multiple items - ask which to pick up. */
		Vector pickup = v_init (sizeof(int), 20);
	
		/* Do the asking */
		mask_vec (n, pickup, ground);
		v_free (ground);
	
		/* Put items in ret_list into inventory. The loop
		* continues until ret_list is done or the pack is full. */
		for (i = 0; i < pickup->len; ++ i)
		{
			/* Pick up the item; quit if the bag is full */
			if (!pack_add (&pmons.pack, &THING(n, *(int*)v_at (pickup, i))->thing.item))
				break;
			/* Remove item from main play */
			rem_ref (n, i);
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
	mons_eat (player, food);
	return 1;
}

int Ksdrop ()
{
	struct Item *drop = player_use_pack ("Drop what?", ITCAT_ALL);
	if (drop == NULL)
		return 0;
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
	show_contents (pmons.pack, ITCAT_ALL);
	return 0;
}

int Knlook ()
{
	int k = 0;
	int n = to_buffer (player->yloc, player->xloc);

	LOOP_THING(n, i)
	{
		struct Thing *th = THING(n, i);
		if (th->type != THING_ITEM)
			continue;

		char *line = get_inv_line (&THING(n, i)->thing.item);
		pline ("You%s see here %s. ", ((k++) ? " also" : ""), line);
		free (line);
	}
	if (k == 0)
		pline("You see nothing here. ");

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

	unsigned u = PACK_AT (get_Itref (pmons.pack, wield));
	pmons.pack.items[u] = NULL;
	new_thing (THING_ITEM, cur_dlevel, player->yloc, player->xloc, wield);

	if (mons_unwield (player))
		mons_wield (player, wield);

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

int key_lookup (uint32_t key)
{
	int i;
	char ch = (char) key;
	for (i = 0; i < NUM_KEYS; ++ i)
	{
		if (ch == (char)(Keys[i].key&0xff) && gr_equiv (key, Keys[i].key))
			return (*Keys[i].action) ();
	}
	pline ("Unknown command '%s%c'.",
	       (escape(ch) == ch ? "" : "^"),
		   escape(ch));
	return 0;
}

