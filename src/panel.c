/* panel.c */

#include "include/panel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/words.h"
#include "include/loop.h"
#include "include/dlevel.h"

#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>

int p_height, p_width;
int sb_width;
glyph *sidebar = NULL;

Vector messages = NULL;

void p_pane ()
{
	int i, j;
	int xpan = 0, ypan = txt_h - PANE_H;
	txt_clear ();
	/*for (i = 0; i < p_height; ++ i)
		for (j = 0; j < p_width; ++ j)
			txt_mvaddch (ypan + i, xpan + j, ' ');*/
	txt_box (ypan, xpan, p_height-1, p_width-1);

	int max = txt_h;

	for (i = 0; i < max; ++ i)
	{
		int curHP = (pmons.HP_max*i)/max;
		if (curHP > pmons.HP)
		{
			txt_mvaddch (max - i - 1, txt_w - 1, ' ');
			continue;
		}
		int gpart = (16*i)/max, rpart = (16*(max-i))/max;
		gpart = (gpart == 0) ? 0 : gpart - 1;
		rpart = (rpart == 0) ? 0 : rpart - 1;
		txt_mvaddch (max - i - 1, txt_w - 1, COL_BG_RED(rpart) | COL_BG_GREEN(gpart) | ' ');
	}

	/*for (i = 0; i < max; ++ i)
	{
		int curXP = ((level_sum[pmons.level+1] - level_sum[pmons.level])*i)/max;
		if (curXP + level_sum[pmons.level] >= pmons.exp || i == max-1)
			txt_mvaddch (max - i - 1, txt_w - 2, ' ');
		else
			txt_mvaddch (max - i - 1, txt_w - 2, COL_BG_BLUE(10) | ' ');
	}*/

	txt_mvprint (0, 0, "%lu", Time);

	//txt_mvprint (ypan + 1, xpan + 1, "%s", w_short (pmons.name + 1, p_width - 3), get_rank ());
	txt_mvprint (ypan + 2, xpan + 1, "HP %d:%d", pmons.HP, pmons.HP_max);
	txt_mvprint (ypan + 3, xpan + 1, "LV %d:%d", pmons.level, pmons.exp);
	/*char *rank = get_rank ();
	int rlen = strlen (rank);
	txt_mvprint (ypan + 1, xpan + p_width - 1 - rlen, rank);*/

	if (sb_width == 0)
		goto skip1;
	for (i = 0; i < map_graph->vh; ++ i)
	{
		txt_mvaddch (i, map_graph->vw, ACS_VLINE);
		for (j = map_graph->vw + 1; j < txt_w - 1; ++ j)
		{
			txt_mvaddch (i, j, sidebar[sb_buffer(i, j-map_graph->vw)]);
		}
	}
skip1:
	if (messages == NULL)
		goto skip2;
	for (i = 0; i < 10 && i < messages->len; ++ i)
	{
		char *msg = v_at (messages, messages->len - i - 1);
		int len = strlen (msg);
		txt_mvprint (2*txt_h/3 + i, (txt_w - len)/2, msg);
	}
skip2:
	;
}

void p_sidebar (int width)
{
	int i, j;
	forced_refresh = 1;
	if (width == 0)
	{
		for (i = 0; i < map_graph->vh; ++ i)
		{
			for (j = map_graph->vw; j < txt_w - 1; ++ j)
				txt_mvaddch (i, j, 0);
		}
		map_graph->vw += sb_width;
		sb_width = 0;
		return;
	}
	if (sb_width != 0)
		p_sidebar (0);
	sb_width = width;
	map_graph->vw -= sb_width;
	sidebar = realloc (sidebar, sizeof(glyph) * sb_width * map_graph->vh);
	for (i = 0; i < sb_width * map_graph->vh; ++ i)
		sidebar[i] = ' ';
}

void sb_baddch (int buf, glyph gl)
{
	sidebar[buf] = gl;
}

void sb_mvaddch (int yloc, int xloc, glyph gl)
{
	sb_baddch (sb_buffer (yloc, xloc), gl);
}

void sb_mvprint (int yloc, int xloc, char *str, ...)
{
}

int sb_buffer (int yloc, int xloc)
{
	return sb_width*yloc + xloc;
}

void p_init ()
{
	if (!messages)
		messages = v_dinit (1024);

	map_graph->vh = txt_h - PANE_H;
	map_graph->vw = txt_w - 2;

	p_height = PANE_H;
	p_width = map_graph->w;
/*
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
{
	va_list args;
	char out[1024];

	va_start (args, str);
	vsprintf (out, str, args);
	va_end (args);

	v_pstr (messages, out);
}

char p_ask (char *results, char *question)
{
	return results[0];
}

void p_lines (Vector lines)
{
}

