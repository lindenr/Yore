/* magic.c */

#include "include/magic.h"
#include "include/dlevel.h"
#include "include/thing.h"
#include "include/player.h"
#include "include/pixel.h"
#include "include/panel.h"

#include <stdio.h>
#include <malloc.h>

Vector sp_list = NULL;
Vector pl_runes = NULL;

void sp_shield (struct Thing *from, int yloc, int xloc)
{
	struct M_shield shield = {SP_SHIELD, yloc, xloc, 2};
	v_push (sp_list, &shield);
	px_mvaddbox (yloc, xloc, BOX_MAGIC, 0);
}

void sp_player_shield ()
{
	int yloc, xloc;
	pl_mvchoose (&yloc, &xloc, "Choose something to shield.", "Cast shield here?");
	sp_shield (player, yloc, xloc);
}

struct Spelltype all_spells[] = {
	{"shield", NULL, &sp_player_shield}
};

#define SP_VEC(i,a,b) temp = (struct vector_){(a), 10, (b), (b)};\
all_spells[i].runes = malloc(sizeof(temp));\
memcpy(all_spells[i].runes, &temp, sizeof(temp));
void sp_init ()
{
	struct vector_ temp;
	SP_VEC (0, "000010200\0""001000000", 2);
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
			panic ("unknown spell in sp_getloc");
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
				panic ("unknown spell in sp_tick");
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
	csr_move (ystart, xstart);
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

/*
#define NUM_SPELLS ((sizeof(spells) / sizeof(*spells))-1)
#define SPELL(c,t,p) {c,t,p}

struct Spell spells[] = {
	SPELL('f', SP_ATKF, 2),
	SPELL('a', SP_ATKA, 2),
	SPELL('e', SP_ATKE, 2),
	SPELL('w', SP_ATKW, 2),
	SPELL('F', SP_DEFF, 2),
	SPELL('A', SP_DEFA, 2),
	SPELL('E', SP_DEFE, 2),
	SPELL('W', SP_DEFW, 2),
	SPELL(0,   0,       0)
};
/ *
	JEWEL_2F, 
	JEWEL_2W,
	JEWEL_2E,
	JEWEL_2A,

	JEWEL_4F,
	JEWEL_4W,
	JEWEL_4E,
	JEWEL_4A,* /

void magic_alter (struct Spell *sp, enum JEWEL_TYPE jwl)
{
	switch (jwl)
	{
		case JEWEL_2_:
			++ sp->power;
		case JEWEL_1_:
			++ sp->power;
			break;
		if (1) printf("sdfg");
		case JEWEL_4F:
		{
			//
		}
		default:
			break;
	}
}

struct Spell *magic_create (char c)
{
	int i;
	for (i = 0; i < NUM_SPELLS; ++ i)
	{
		if (c == spells[i].ch) break;
	}
	if (i >= NUM_SPELLS) return NULL;
	
	struct Spell *sp = malloc (sizeof(*sp));
	memcpy (sp, &(spells[i]), sizeof(*sp));
	return sp;
}

void magic_apply (struct Thing *from, struct Spell *spell)
{
}

bool magic_plspell (char c)
{
	int i;
	struct Spell *sp = magic_create (c);
	if (sp == NULL) return false;
	for (i = 0; i < BELT_JEWELS; ++ i)
		magic_alter (sp, U.jewel[i]);
	magic_apply (player, sp);
	return true;
}

bool magic_isspell (char c)
{
	int i;
	for (i = 0; i < NUM_SPELLS; ++ i)
	{
		if (c == spells[i].ch) return true;
	}
	return false;
}*/

