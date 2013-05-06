/* magic.c */

#include "include/magic.h"
#include "include/dlevel.h"
#include "include/thing.h"
#include "include/player.h"
#include "include/pixel.h"
#include "include/panel.h"
#include "include/vision.h"

#include <stdio.h>
#include <malloc.h>

Vector sp_list = NULL;
Vector pl_runes = NULL;

void sp_bolt (struct Thing *from, int yloc, int xloc)
{
	projectile (from, "bolt", 0, 10);
	bres_callback (from->yloc, from->xloc, &pr_at);
	bres_draw (yloc, xloc);
}

void sp_player_bolt ()
{
	int yloc, xloc;
	pl_mvchoose (&yloc, &xloc, "Choose a target.", "Fire bolt at this tile?");
	sp_bolt (player, yloc, xloc);
}

void sp_explosion (struct Thing *from, int yloc, int xloc)
{
	struct DLevel *dlevel = dlv_lvl (from->dlevel);
	int j, k;
	for (k = -1; k < 2; ++ k) for (j = -1; j < 2; ++ j)
	{
		struct Thing *th;
		int n = gr_buffer (yloc+k, xloc+j);
		LOOP_THING (dlevel->things, n, i)
		{
			th = THING(dlevel->things, n, i);
			if (th->type == THING_MONS)
				break;
		}
		if (i < dlevel->things[n]->len)
			mons_blast (from, th, 2 - ((!!j)|(!!k)));
	}
}

void sp_player_explosion ()
{
	int yloc, xloc;
	pl_mvchoose (&yloc, &xloc, "Choose a target.", "Cast explosion here?");
	sp_explosion (player, yloc, xloc);
}

void sp_sling (struct Thing *from, int yloc, int xloc)
{
	projectile (from, "stone", 3, 10);
	bres_callback (from->yloc, from->xloc, &pr_at);
	bres_draw (yloc, xloc);
}

void sp_player_sling ()
{
	int yloc, xloc;
	pl_mvchoose (&yloc, &xloc, "Choose a target.", "Sling at this tile?");
	sp_sling (player, yloc, xloc);
}

void sp_shield (struct Thing *from, int yloc, int xloc)
{
	struct M_shield shield = {SP_SHIELD, yloc, xloc, 2};
	v_push (sp_list, &shield);
	px_mvaddbox (yloc, xloc, BOX_MAGIC, 0);
}

void sp_player_shield ()
{
	int yloc, xloc;
	pl_mvchoose (&yloc, &xloc, "Choose a target.", "Cast shield here?");
	sp_shield (player, yloc, xloc);
}

int sp_protected (struct Thing *from, int yloc, int xloc)
{
	int i;
	if (!sp_list)
		return 0;
	for (i = 0; i < sp_list->len; ++ i)
	{
		union Spell *spell = v_at (sp_list, i);
		if (spell->type == SP_SHIELD &&
		    spell->shield.yloc == yloc &&
		    spell->shield.xloc == xloc)
			return 1;
	}
	return 0;
}

struct Spelltype all_spells[] = {
	{"shield", NULL, &sp_player_shield},
	{"sling", NULL, &sp_player_sling},
	{"explosion", NULL, &sp_player_explosion},
	{"bolt", NULL, &sp_player_bolt}
};

#define SP_VEC(a) temp = (struct vector_){(a), 10, sizeof(a)/10, sizeof(a)/10};\
all_spells[i].runes = malloc(sizeof(temp));\
memcpy(all_spells[i].runes, &temp, sizeof(temp));\
i++
void sp_init ()
{
	struct vector_ temp;
	int i = 0;
	SP_VEC ("000010200\0""001000000");
	SP_VEC ("000021000\0");
	SP_VEC ("010000002\0""000010200\0""100000000");
	SP_VEC ("010000002\0""000100200\0""010000002");
}

int sp_getloc (union Spell *spell, int *yloc, int *xloc)
{
	switch (spell->type)
	{
		case SP_SHIELD:
		{
			*yloc = spell->shield.yloc;
			*xloc = spell->shield.xloc;
			return 1;
		}
		default:
		{
			return 0;
		}
	}
}

void sp_tick ()
{
	int i, yloc, xloc;
	for (i = 0; i < sp_list->len; ++ i)
	{
		if (sp_getloc (v_at (sp_list, i), &yloc, &xloc))
			px_mvrembox (yloc, xloc, BOX_MAGIC);
	}
	for (i = 0; i < sp_list->len; ++ i)
	{
		union Spell *spell = v_at (sp_list, i);
		switch (spell->type)
		{
			case SP_SHIELD:
			{
				struct M_shield *shield = &spell->shield;
				if (shield->turns)
				{
					-- shield->turns;
					break;
				}
				v_rem (sp_list, i);
				-- i;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	for (i = 0; i < sp_list->len; ++ i)
	{
		if (sp_getloc (v_at (sp_list, i), &yloc, &xloc))
			px_mvaddbox (yloc, xloc, BOX_MAGIC, 0);
	}
}

void pl_cast ()
{
	int i, j;
	for (j = 0; j < NUM_SPELLS; ++ j)
	{
		struct Spelltype *sp = &all_spells[j];
		Vector runes = sp->runes;
		if (pl_runes->len < runes->len)
			continue;
		for (i = 0; i < runes->len; ++ i)
		{
			if (strcmp (v_at (runes, i), v_at (pl_runes, pl_runes->len - runes->len + i)))
				break;
		}
		if (i >= runes->len)
			break;
	}
	if (j >= NUM_SPELLS)
		return;
	p_msg ("You cast %s!", all_spells[j].name);
	p_pane ();
	gr_refresh ();
	all_spells[j].player_action ();
}

Rune sp_rune (int siz)
{
	int ystart = (pnumy-siz)/2, xstart = (pnumx-siz)/2;
	txt_fbox (ystart-1, xstart-1, siz+1, siz+1, ' ');
	csr_show ();
	csr_move (ystart+(siz/2), xstart+(siz/2));
	gr_noecho ();
	csr_noblink ();
	Rune rune = malloc (siz*siz+1);
	memset (rune, '0', siz*siz);
	rune[siz*siz] = '\0';
	int ymove, xmove;
	uint32_t key = ' ';
	char sp = '1';
	while (key != CH_LF && key != CH_CR && key != CH_ESC)
	{
		key = pl_move (&ymove, &xmove, gr_getfullch ());
		//printf("%d\n", key);
		if (key == ' ')
		{
			int dy = csr_y - ystart, dx = csr_x - xstart;
			if (rune[dy*siz + dx] > '0')
				continue;
			rune[dy*siz + dx] = sp;
			txt_mvaddch (csr_y, csr_x, sp);
			gr_refresh ();
			sp ++;
			continue;
		}
		if (ymove == -1 && csr_y > ystart)
			csr_move (csr_y-1, csr_x);
		else if (ymove == 1 && csr_y < ystart+siz-1)
			csr_move (csr_y+1, csr_x);
		if (xmove == -1 && csr_x > xstart)
			csr_move (csr_y, csr_x-1);
		else if (xmove == 1 && csr_x < xstart+siz-1)
			csr_move (csr_y, csr_x+1);
	}
	//txt_fbox (ystart-1, xstart-1, siz+1, siz+1, '\0');
	int i, j;
	for (i = ystart-1; i <= ystart+siz; ++ i) for (j = xstart-1; j <= xstart+siz; ++ j)
		txt_mvaddch (i, j, '\0');
	gr_refresh ();
	csr_move (player->yloc - cam_yloc, player->xloc - cam_xloc);
	if (key == CH_ESC)
		return NULL;
	return rune;
}

