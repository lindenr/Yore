/* panel.c */

#include "include/panel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/words.h"
#include "include/loop.h"
#include "include/dlevel.h"
#include "include/vision.h"
#include "include/skills.h"

#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>

int p_height, p_width;
int sb_width;
glyph *sidebar = NULL;

Vector messages = NULL;

Graph gpan = NULL;

void p_pane ()
{
	int i;
	int xpan = 0, ypan = gr_h - PANE_H;
	if (!gpan)
	{
		gpan = gra_init (p_height, p_width, ypan, xpan, p_height, p_width);
		gpan->def = COL_PANEL;
	}
	/*for (i = 0; i < p_height; ++ i)
		for (j = 0; j < p_width; ++ j)
			txt_mvaddch (ypan + i, xpan + j, ' ');*/
	gra_fbox (gpan, 0, 0, p_height-1, p_width-1, ' ');

	//int max = gr_h;
	
	/* TODO health bar in its own graph
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
	} */

	/*for (i = 0; i < max; ++ i)
	{
		int curXP = ((level_sum[pmons.level+1] - level_sum[pmons.level])*i)/max;
		if (curXP + level_sum[pmons.level] >= pmons.exp || i == max-1)
			txt_mvaddch (max - i - 1, txt_w - 2, ' ');
		else
			txt_mvaddch (max - i - 1, txt_w - 2, COL_BG_BLUE(10) | ' ');
	}*/

	gra_mvprint (gpan, 1, 1, "T %lu", Time);

	gra_mvprint (gpan, 2, 1, "%s the Player", w_short (pmons.name + 1, 20));
	gra_mvprint (gpan, 3, 1, "HP %d/%d (+%.1f)", pmons.HP, pmons.HP_max, pmons.HP_rec);
	gra_mvprint (gpan, 4, 1, "LV %d:%d/infinity", pmons.level, pmons.exp); // TODO?
	/*char *rank = get_rank ();
	int rlen = strlen (rank);
	txt_mvprint (ypan + 1, xpan + p_width - 1 - rlen, rank);*/

	if (sb_width == 0)
		goto skip1;
	/*for (i = 0; i < map_graph->vh; ++ i)
	{
		txt_mvaddch (i, map_graph->vw, ACS_VLINE);
		for (j = map_graph->vw + 1; j < txt_w - 1; ++ j)
		{
			txt_mvaddch (i, j, sidebar[sb_buffer(i, j-map_graph->vw)]);
		}
	}*/
skip1:
	if (messages == NULL)
		goto skip2;
	for (i = 0; i < 5 && i < messages->len; ++ i)
	{
		struct P_msg *msg = v_at (messages, messages->len - i - 1);
		int len = strlen (msg->msg);
		gra_mvprint (gpan, 1 + i, (gr_w - len)/2, msg->msg);
	}
skip2:
	;
}
/*
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
}*/

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
		messages = v_dinit (sizeof(struct P_msg));

	//map_graph->vh = txt_h - PANE_H;
	//map_graph->vw = txt_w - 1;

	p_height = PANE_H;
	p_width = map_graph->vw;
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

void p_amsg (const char *str)
{
	struct P_msg msg;
	msg.expiry = Time+1;
	strncpy (msg.msg, str, P_MSG_LEN-1);
	msg.msg[P_MSG_LEN-1] = 0;
	v_push (messages, &msg);
}

void p_msg (const char *str, ...)
{
	va_list args;
	char out[100];

	snprintf(out, 10, "(%lu) ", Time);

	va_start (args, str);
	vsnprintf (out + strlen(out), 90, str, args);
	va_end (args);

	p_amsg (out);
}

char p_ask (const char *results, const char *question)
{
	p_amsg (question);
	p_pane ();
	char in;
	do
		in = (char)gr_getch();
	while (!strchr(results, in));
	return in;
}

char p_lines (Vector lines)
{
	int i;
	int max = 0;
	for (i = 0; i < lines->len; ++ i)
	{
		int len = strlen(v_at(lines, i));
		if (len > max)
			max = len;
	}

	int h = lines->len + 2, w = max+4;
	int yloc = (gr_h - h)/2, xloc = (gr_w - w)/2;

	Graph box = gra_init (h, w, yloc, xloc, h, w);
	gra_fbox (box, 0, 0, h-1, w-1, ' ');
	
	for (i = 0; i < lines->len; ++ i)
	{
		char *str = v_at(lines, i);
		if (str[0] != '#')
		{
			gra_mvaprint (box, i+1, 2, str);
			continue;
		}
		gra_mvprint (box, i+1, (w-strlen(str)+1)/2, str+1);
	}
	gra_box (box, 0, 0, h-1, w-1);
	char ret = gr_getch();

	gra_free (box);
	
	return ret;
}

// status screen stuff
// ideally want to be able to access skills etc from the status window
int p_status (enum PanelType type)
{
	switch (type)
	{
		case STATUS_TYPE:
			break;
		case STATUS_SKILLS:
			int ret = p_skills (type);
			if (ret)
				return ret;
			break;
	}
	int h = 20, w = 81;
	int y = (gr_h - h)/2 - 10, x = (gr_w - w)/2;
	Graph panel = gra_init (h, w, y, x, h, w);
	panel->def = COL_STATUS;
	gra_fbox (panel, 0, 0, h-1, w-1, ' ');
	gra_cprint (panel, 2, "STATUS SCREEN");
	gra_mvprint (panel, 0, w-5, "(Esc)");
	gra_cprint (panel, 4, "Name: %12s  %c  Race:  %s       ", pmons.name+1, ACS_VLINE, "Human");
	gra_cprint (panel, 5, "  HP: %d/%d (%+.1f)  %c  ST:    %d/%d (%+.1f)",
				pmons.HP, pmons.HP_max, pmons.HP_rec, ACS_VLINE, pmons.ST, pmons.ST_max, pmons.ST_rec);
	gra_cprint (panel, 6, "LV %d:%d/infinity  %c  Speed: %d       ", pmons.level, pmons.exp, ACS_VLINE, pmons.speed);

	while (gr_getch() != CH_ESC) {}
	gra_free (panel);
	return 0;
}

int p_skills (enum PanelType type)
{
	/*int y, x; 
	pl_mvchoose (&y, &x, "Charge where?", NULL, 1);
	if (y == -1)
		return 0;
	sk_charge (player, y, x);
	return 1;*/
	int h = 10, w = 41;
	int y = (gr_h - h)/2, x = (gr_w - w)/2;
	Graph panel = gra_init (h, w, y, x, h, w);
	panel->def = COL_STATUS;
	gra_fbox (panel, 0, 0, h-1, w-1, ' ');
	gra_cprint (panel, 2, "SKILLS");
	gra_cprint (panel, 3, "(enter to inspect; letter to use)");
	gra_mvprint (panel, 0, w-5, "(Esc)");
	if (pmons.skills->len == 0)
	{
		gra_cprint (panel, 4, "(no skills available)");
		while (gr_getch() != CH_ESC) {}
		gra_free (panel);
		return 0;
	}
	
	int i, selected = 0;
	int j;
	while (1)
	{
		char letter = 'a';
		for (i = 0; i < pmons.skills->len; ++ i)
		{
			Skill sk = v_at (pmons.skills, i);
			int row = 4+i;
			gra_cprint (panel, row, "%c - %s %d:%d", letter, sk_name(sk), sk->level, sk->exp);
			++ letter;
		}
		for (j = 1; j < w-1; ++ j)
			gra_invert (panel, 4+selected, j);
		char in = gr_getch();
		for (j = 1; j < w-1; ++ j)
			gra_invert (panel, 4+selected, j);
		if (in == CH_ESC)
			break;
		else if ((in == GRK_UP /*|| in == 'k'*/) && selected > 0)
			-- selected;
		else if ((in == GRK_DN /*|| in == 'j'*/) && selected < pmons.skills->len-1)
			++ selected;
		else if (in == CH_LF || in == CH_CR)
		{
			gra_clear (panel);
			p_mvchoose (&y, &x, "Charge where?", NULL, 1);
			if (y == -1)
				continue;
			gra_free (panel);
			sk_charge (player, y, x, v_at (pmons.skills, selected));
			return 1;
		}
		else if (in >= 'a' && in < letter)
		{
			int n = in - 'a';
			sk_use (player, v_at(player->skills, n));
		}
	}
	gra_free (panel);
	return 0;
}

Graph overlay = NULL;
int path_hit (struct DLevel *dlevel, int y, int x)
{
	gra_mvaddch (overlay, y-map_graph->cy, x-map_graph->cx, ' '|COL_BG_RED(8));
	//gra_highlight (map_graph, y, x);
	return 1;
}

void p_mvchoose (int *yloc, int *xloc, const char *instruct, const char *confirm, int showpath)
{
	int orig_y = map_graph->csr_y, orig_x = map_graph->csr_x;
	if (showpath)
	{
		overlay = gra_init (gr_h, gr_w, 0, 0, gr_h, gr_w);
		gra_clear (overlay);
	}
	if (instruct)
		p_msg (instruct);
	p_pane ();
	gr_refresh ();
	int xmove, ymove;
	uint32_t key = p_move (&ymove, &xmove, gr_getfullch ());
	while (key != '.' || (confirm && (p_ask ("yn", confirm) != 'y')))
	{
		if (ymove == -1 && map_graph->csr_y > 0)
			gra_cmove (map_graph, map_graph->csr_y-1, map_graph->csr_x);
		else if (ymove == 1 && map_graph->csr_y < map_graph->h-1)
			gra_cmove (map_graph, map_graph->csr_y+1, map_graph->csr_x);
		if (xmove == -1 && map_graph->csr_x > 0)
			gra_cmove (map_graph, map_graph->csr_y, map_graph->csr_x-1);
		else if (xmove == 1 && map_graph->csr_x < map_graph->w-1)
			gra_cmove (map_graph, map_graph->csr_y, map_graph->csr_x+1);
		//if (gra_nearedge (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x))
		//	gra_centcam (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x);
		if (showpath)
		{
			gra_clear (overlay);
			bres_draw (player->yloc, player->xloc, NULL, dlv_attr(player->dlevel), &path_hit, map_graph->csr_y, map_graph->csr_x);
		}
		key = p_move (&ymove, &xmove, gr_getfullch ());
	}
	*yloc = map_graph->csr_y;
	*xloc = map_graph->csr_x;
	gra_cmove (map_graph, orig_y, orig_x);
	gra_free (overlay);
}

uint32_t p_move (int *ymove, int *xmove, uint32_t key)
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

