/* panel.c */

#include "include/panel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/words.h"
#include "include/rank.h"

#include <stdlib.h>
#include <malloc.h>

int p_width, p_height;
int p_open = 0;

char *p_tabs[NUM_TABS] = {NULL, };
Vector messages = NULL;

void p_pane ()
{
	p_height = snumy - pnumy, p_width = pnumx;
	int i, j;
	int xpan = 0, ypan = pnumy;
	for (i = 0; i < p_height; ++ i)
		for (j = 0; j < p_width; ++ j)
			txt_mvaddch (ypan + i, xpan + j, ' ');
	txt_box (ypan, xpan, p_height-1, p_width-1);

	int max = snumy;

	for (i = 0; i < max; ++ i)
	{
		int curHP = (pmons.HP_max*i)/max;
		if (curHP > pmons.HP)
		{
			txt_mvaddch (max - i - 1, pnumx, ' ');
			continue;
		}
		int gpart = (16*i)/max-1, rpart = (16*(max-i))/max-1;
		gpart = (gpart < 0) ? 0 : gpart;
		rpart = (rpart < 0) ? 0 : rpart;
		txt_mvaddch (max - i - 1, pnumx, COL_BG_RED(rpart) | COL_BG_GREEN(gpart) | ' ');
	}

	txt_mvprint (ypan + 1, xpan + 1, "%s", w_short (pmons.name + 1, p_width - 3), get_rank ());
	txt_mvprint (ypan + 2, xpan + 1, "HP %d:%d", pmons.HP, pmons.HP_max);
	txt_mvprint (ypan + 3, xpan + 1, "LV %d:%d", pmons.level, pmons.exp);
	char *rank = get_rank ();
	int rlen = strlen (rank);
	txt_mvprint (ypan + p_height - 2, xpan + p_width - 1 - rlen, rank);
}

void p_init ()
{/*
	int i;
	if (!messages)
		messages = v_dinit (1024);

	for (i = 0; i < NUM_TABS; ++ i)
	{
		p_tabs[i] = realloc (p_tabs[i], p_width * (glnumy - p_height));
		memset (p_tabs[i], ' ', p_width * (glnumy - p_height));
	}*/
}

void p_tab (int state)
{/*
	int i, j;
	p_open = state;

	if (state == 0)
	{
		for (j = glnumx - p_width; j < glnumx; ++ j)
			txt_mvaddch (p_height, j, ' ');
		for (i = p_height+1; i < glnumy; ++ i)
		{
			for (j = glnumx - p_width; j < glnumx; ++ j)
				txt_mvaddch (i, j, 0); / * clear it * /
		}
		return;
	}

	p_update ();*/
}

void p_update ()
{/*
	int i, j;
	if (p_open <= 0 || p_open > NUM_TABS)
		return;

	for (j = glnumx - p_width; j < glnumx; ++ j)
		txt_mvaddch (p_height, j, ACS_HLINE);
	for (i = p_height+1; i < glnumy; ++ i)
	{
		for (j = glnumx - p_width; j < glnumx; ++ j)
			txt_mvaddch (i, j, p_tabs[p_open][(i - p_height)*p_width + j - glnumx]);
	}*/
}

void p_messages_display ()
{/*
	if (!messages)
		return;

	int i;
	for (i = 0; i < messages->len; ++ i)
	{
	}*/
}

void p_msg (char *str, ...)
{/*
	va_list args;
	char out[1024];

	va_start (args, str);
	vsprintf (out, str, args);
	va_end (args);

	v_pstr (messages, out);
	p_update ();*/
}

char p_ask (char *results, char *question)
{
	return results[0];
}

void p_lines (Vector lines)
{
}

