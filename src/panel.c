/* panel.c */

#include "include/panel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/words.h"
#include "include/dlevel.h"
#include "include/drawing.h"
#include "include/skills.h"
#include "include/event.h"
#include "include/player.h"
#include "include/drawing.h"
#include "include/string.h"

#include <stdlib.h>
#include <stdarg.h>

int p_height, p_width;

Vector messages = NULL;

Graph gpan = NULL, gpred = NULL;

struct Monster *cur_player = NULL;

void p_timeline ()
{
	int i;
	int ticks_per_tile = 25;
	gra_mvaddch (gpred, 0, 0, ACS_TTEE | COL_TXT_BRIGHT);
	for (i = 1; i < gr_h; ++ i)
		gra_mvaddch (gpred, i, 0, ACS_VLINE | COL_TXT_BRIGHT);
	int y, x;
	for (i = 0; i < cur_dlevel->playerIDs->len; ++ i)
	{
		MID *plID = v_at (cur_dlevel->playerIDs, i);
		struct Monster *pl = MTHIID (*plID);
		for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
		{
			int w = map_buffer (pl->yloc + y, pl->xloc + x);
			if (w == -1)
				continue;
			MID ID = cur_dlevel->monsIDs[w];
			if (!ID)
				continue;
			struct Monster *mons = MTHIID (ID);
			if (!mons)
				continue;
			if (mons->status.moving.ydir || mons->status.moving.xdir)
				gra_mvaddch (gpred, (mons->status.moving.arrival - curtick) / ticks_per_tile, 0,
					ACS_PLUS | COL_TXT(15,15,0));
			else if (mons->status.attacking.ydir || mons->status.attacking.xdir)
				gra_mvaddch (gpred, (mons->status.attacking.arrival - curtick) / ticks_per_tile, 0,
					ACS_PLUS | COL_TXT(15,0,0));
		}
		gra_mvaddch (gpred, (pl->speed+1)/ticks_per_tile, 0, 'M' | COL_TXT(0,15,0));
		gra_mvaddch (gpred, (pl->speed/2)/ticks_per_tile, 0, 'P' | COL_TXT(0,15,0));
		gra_mvaddch (gpred, (pl->speed/3)/ticks_per_tile, 0, 'E' | COL_TXT(0,15,0));
	}
}

void p_pane (struct Monster *player)
{
	int i;
	int xpan = 0, ypan = gr_h - PANE_H;
	if (!player)
		player = cur_player;
	cur_player = player;
	if (!gpan)
	{
		gpan = gra_init (p_height, p_width, ypan, xpan, p_height, p_width);
		gpan->def = COL_PANEL;
	}
	gra_fbox (gpan, 0, 0, p_height-1, p_width-1, ' ');

	if (!gpred)
	{
		gpred = gra_init (gr_h, 1, 0, gr_w - 1, gr_h, 1);
	}
	p_timeline ();

	//int max = gr_h;

	/* TODO health bar in its own graph
	for (i = 0; i < max; ++ i)
	{
		int curHP = (player.HP_max*i)/max;
		if (curHP > player.HP)
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
		int curXP = ((level_sum[player.level+1] - level_sum[player.level])*i)/max;
		if (curXP + level_sum[player.level] >= player.exp || i == max-1)
			txt_mvaddch (max - i - 1, txt_w - 2, ' ');
		else
			txt_mvaddch (max - i - 1, txt_w - 2, COL_BG_BLUE(10) | ' ');
	}*/

	gra_mvprint (gpan, 1, 3, "T %llu", curtick);
	gra_mvaddch (gpan, 1, 1, '>' | COL_TXT(15,7,0));

	if (player)
	{
		int exp_needed = mons_exp_needed (player->level);
		gra_mvprint (gpan, 2, 3, "%s the Player", w_short (player->name, 20));
		gra_mvprint (gpan, 3, 3, "Health  %d (%d)", player->HP, player->HP_max);
		gra_mvprint (gpan, 4, 3, "Stamina %d (%d)", player->ST, player->ST_max);
		gra_mvprint (gpan, 5, 3, "Power   %d (%d)", player->MP, player->MP_max);
		gra_mvprint (gpan, 6, 3, "Armour  %d", player->armour);
		gra_mvprint (gpan, 7, 3, "Level   %d", player->level);
		if (exp_needed != -1)
			gra_mvprint (gpan, 8, 3, "Exp     %d (%d)", player->exp, exp_needed);
		else
			gra_mvprint (gpan, 8, 3, "Exp     %d (%c)", player->exp, 0xEC);
		gra_mvaddch (gpan, 2, 1, '@' | COL_TXT(15,15,15));
		gra_mvaddch (gpan, 3, 1, GL_HEALTH);
		gra_mvaddch (gpan, 4, 1, GL_STAMINA);
		gra_mvaddch (gpan, 5, 1, GL_POWER);
		gra_mvaddch (gpan, 6, 1, '[' | COL_TXT(11,11,0));
		gra_mvaddch (gpan, 7, 1, 6 | COL_TXT(7,15,15));
		gra_mvaddch (gpan, 8, 1, 0xE4 | COL_TXT(15,0,15));
		gra_mvprint (gpan, 2, 100, "SHIELD Y:%d X:%d", player->status.defending.ydir, player->status.defending.xdir);
		if (player->status.bleeding)
		{
			gpan->def = COL_TXT(15,0,0);
			gra_mvprint (gpan, 3, 100, "BLEEDING");
			gpan->def = COL_TXT_DEF;
		}
		if (player->status.charging)
			gra_mvprint (gpan, 3, 110, "CHARGING");
	}

	if (pl_focus_mode)
		gra_mvprint (gpan, 1, 110, "FOCUS MODE");

	if (messages == NULL)
		goto skip2;
	for (i = 0; i < 8 && i < messages->len; ++ i)
	{
		struct P_msg *msg = v_at (messages, messages->len - i - 1);
		int len = P_MSG_LEN; //strlen (msg->msg);
		gra_mvaprintex (gpan, 8 - i, (gr_w - len)/2, msg->msg);
	}
skip2:
	;
}

void p_init ()
{
	if (!messages)
		messages = v_dinit (sizeof(struct P_msg));

	p_height = PANE_H;
	p_width = map_graph->vw;
}

void p_msgbox (const char *msg)
{
	Vector lines = w_lines (msg, 40);
	v_pstr (lines, "");
	v_pstr (lines, "#n000BBB00[OK]");
	p_lines (lines);
	v_free (lines);
}

void p_amsg (const char *str)
{
	Vector formatted = p_formatted (str, P_MSG_LEN);
	struct P_msg msg;
	int i;
	for (i = 0; i < formatted->len; ++ i)
	{
		struct MenuOption *m = v_at (formatted, i);
		memcpy (msg.msg, m->ex_str, sizeof(glyph)*m->len);
		msg.msg[P_MSG_LEN-1] = 0;
		v_push (messages, &msg);
	}
	p_ffree (formatted);
}

void p_msg (const char *str, ...)
{
	va_list args;
	char out[100];

	snprintf(out, 10, "(%llu) ", curtick);

	va_start (args, str);
	vsnprintf (out + strlen(out), 90, str, args);
	va_end (args);

	p_amsg (out);
}

void p_menu_draw (Graph box, Vector lines, int curchoice)
{
	int i;
	for (i = 0; i < lines->len; ++ i)
	{
		struct MenuOption *line = v_at (lines, i);
		if (line->num != -1)
		{
			gra_mvaddch (box, i+1, 2, ACS_PILLAR);
			gra_mvaddch (box, i+1, 3, ' ');
			gra_mvaddch (box, i+1, 4, ' ');
	//		gra_mvaddch (box, i+1, 4, line->letter);
	//		gra_mvaddch (box, i+1, 4, '>');
		}
		if (line->ex_str)
			gra_mvaprintex (box, i+1, 5, line->ex_str);
	}
	if (curchoice != -1)
	{
		gra_mvaddch (box, curchoice+1, 3, '>');
		gra_invert (box, curchoice+1, 2);
		gra_invert (box, curchoice+1, 3);
		//gra_invert (box, curchoice+1, 4);
	}
}

int p_menuex (Vector lines, const char *toquit, int max_line_len)
{
	int i, curchoice = -1;
	int max = max_line_len + 4;

	int h = lines->len + 2, w = max+4;
	int yloc = (gr_h - h)/2, xloc = (gr_w - w)/2;

	Graph box = gra_init (h, w, yloc, xloc, h, w);
	box->def = COL_BG(1,1,1)|COL_TXT_DEF;
	gra_fbox (box, 0, 0, h-1, w-1, COL_TXT_DEF|' ');
	if (toquit)
		gra_mvprint (box, 0, w-strlen(toquit), toquit);
	box->def = COL_TXT_DEF;

	for (i = 0; i < lines->len; ++ i)
	{
		struct MenuOption *line = v_at (lines, i);
		if (line->num != -1)
		{
			curchoice = i;
			break;
		}
	}

	p_menu_draw (box, lines, curchoice);
	if (curchoice == -1)
	{
		p_getch (NULL);
		gra_free (box);
		return -1;
	}
	do
	{
		char ret = p_getch (NULL);
		if (ret == CH_ESC || ret == ' ')
			break;
		if (ret == '\n' || ret == '.')
		{
			gra_free (box);
			struct MenuOption *line = v_at (lines, curchoice);
			return line->num;
		}
		else if (ret == GRK_UP || ret == 'k')
		{
			for (i = curchoice - 1; i >= 0; -- i)
			{
				struct MenuOption *line = v_at (lines, i);
				if (line->num != -1)
					break;
			}
			if (i >= 0)
				curchoice = i;
		}
		else if (ret == GRK_DN || ret == 'j')
		{
			for (i = curchoice + 1; i < lines->len; ++ i)
			{
				struct MenuOption *line = v_at (lines, i);
				if (line->num != -1)
					break;
			}
			if (i < lines->len)
				curchoice = i;
		}
		p_menu_draw (box, lines, curchoice);
	}
	while (1);

	gra_free (box);

	return -1;
}

char p_ask (struct Monster *player, const char *results, const char *question)
{
	p_amsg (question);
	char in;
	do
		in = p_getch (player);
	while (!strchr(results, in));
	return in;
}

char p_getch (struct Monster *pl)
{
	p_pane (pl);
	return gr_getch ();
}

char p_lines (Vector lines)
{
	int i;
	int max = P_MSG_LEN;
	//for (i = 0; i < lines->len; ++ i)
	//{
	//	int len = strlen(v_at(lines, i));
	//	if (len > max)
	//		max = len;
	//}

	int h = lines->len + 2, w = max+4;
	int yloc = (gr_h - h)/2, xloc = (gr_w - w)/2;

	Graph box = gra_init (h, w, yloc, xloc, h, w);
	gra_fbox (box, 0, 0, h-1, w-1, ' ');

	for (i = 0; i < lines->len; ++ i)
	{
		char *str = v_at (lines, i);
		Vector formatted = p_formatted (str, P_MSG_LEN);
		struct MenuOption *m = v_at (formatted, 0);
		if (m->ex_str)
			gra_mvaprintex (box, i+1, 2, m->ex_str);
		p_ffree (formatted);
	}
	gra_box (box, 0, 0, h-1, w-1);
	char ret = p_getch (NULL);

	gra_free (box);

	return ret;
}

/* Status screen */
Graph sc_status = NULL;
int p_status (struct Monster *player, enum PanelType type)
{
	int h = 20, w = 81;
	int y = (gr_h - h)/2 - 10, x = (gr_w - w)/2;
	sc_status = gra_init (h, w, y, x, h, w);
	sc_status->def = COL_STATUS;
	gra_fbox (sc_status, 0, 0, h-1, w-1, ' ');
	gra_cprint (sc_status, 2, "STATUS SCREEN");
	gra_mvprint (sc_status, 0, w-5, "(Esc)");
	gra_cprint (sc_status, 4, "Name: %12s  %c  Race:  %s       ", player->name, ACS_VLINE, mons_typename (player));
	gra_cprint (sc_status, 5, "  HP: %d/%d  %c  ST:    %d/%d",
				player->HP, player->HP_max, ACS_VLINE, player->ST, player->ST_max);
	gra_cprint (sc_status, 6, "LV    %d:%d  %c  Speed: %d", player->level, player->exp, ACS_VLINE, player->speed);
	gra_mvaddch (sc_status, 7, 5, GL_STR | (COL_BG_MASK & sc_status->def));
	gra_mvprint (sc_status, 7, 7, "Str: %d", player->str);
	gra_mvaddch (sc_status, 8, 5, GL_CON | (COL_BG_MASK & sc_status->def));
	gra_mvprint (sc_status, 8, 7, "Con: %d", player->con);
	gra_mvaddch (sc_status, 9, 5, GL_WIS | (COL_BG_MASK & sc_status->def));
	gra_mvprint (sc_status, 9, 7, "Wis: %d", player->wis);
	gra_mvaddch (sc_status, 10, 5, GL_AGI | (COL_BG_MASK & sc_status->def));
	gra_mvprint (sc_status, 10, 7, "Agi: %d", player->agi);
	gra_mvprint (sc_status, 12, 5, "Press 'S' to save.");

	switch (type)
	{
		int ret;
	case P_STATUS:
		break;
	case P_SKILLS:
		ret = p_skills (player, type);
		if (ret > 0)
		{
			gra_free (sc_status);
			return ret;
		}
		else if (ret == -1)
		{
			gra_free (sc_status);
			return 0;
		}
		break;
	}

	while (1)
	{
		char in = p_getch (player);
		if (in == CH_ESC || in == ' ')
			break;
		else if (in == 's')
		{
			int ret = p_skills (player, type);
			if (ret > 0)
			{
				gra_free (sc_status);
				return ret;
			}
			else if (ret == -1)
			{
				gra_free (sc_status);
				return 0;
			}
		}
		else if (in == 'S')
		{
			U.playing = PLAYER_SAVEGAME;
			return -1;
		}
	}
	gra_free (sc_status);
	return 0;
}

glyph output_colours;
/* Skills screen */
int p_skills (struct Monster *player, enum PanelType type)
{
	Vector pskills = player->skills;
	struct String *fmt = str_dinit ();
	str_catf (fmt, "#c#nFFF00000SKILLS#nBBB00000\n%s",
		(pskills->len == 0) ? 
		"#c(no skills available)\n" :
		"#c(press '.' to use)\n");
	
	//char letter = 'a';
	int i;
	for (i = 0; i < pskills->len; ++ i)
	{
		Skill sk = v_at (pskills, i);
		if (sk_isact (sk))
			str_catf (fmt, "#o");
		str_catf (fmt, "#g%s %s %d:%d\n", gl_format (sk_gl(sk)), sk_name(sk), sk->level, sk->exp);
	}
	int n = p_flines (str_data (fmt));
	str_free (fmt);
	if (n == -1)
		return 0;
	else
	{
		int yloc, xloc;
		Skill sk = v_at (pskills, n);
		switch (sk->type)
		{
		case SK_NONE:
			break;
		case SK_CHARGE:
			gra_hide (sc_status);
			if (player->status.charging)
			{
				sk_charge (player, 0, 0, sk);
				return 1;
			}
			output_colours = COL_BG(0,0,0) | COL_TXT(15,15,15);
			p_mvchoose (player, &yloc, &xloc, "Charge where?", NULL, &show_path_on_overlay);
			if (yloc == -1)
			{
				gra_show (sc_status);
				return 0;
			}
			sk_charge (player, yloc, xloc, sk);
			return 1;
		case SK_DODGE:
			break;
		case SK_FIREBALL:
			if (player->MP < 5)
			{
				p_msgbox ("Not enough MP.");
				return 0;
			}
			gra_hide (sc_status);
			output_colours = COL_BG(11,0,0) | COL_TXT(15,15,0);
			p_mvchoose (player, &yloc, &xloc, "Aim where?", NULL, &show_path_on_overlay);
			if (yloc == -1)
			{
				gra_show (sc_status);
				return 0;
			}
			sk_fireball (player, yloc, xloc, sk);
			return 1;
		case SK_WATER_BOLT:
			if (player->MP < 6)
			{
				p_msgbox ("Not enough MP.");
				return 0;
			}
			gra_hide (sc_status);
			output_colours = COL_BG(0,0,8) | COL_TXT(8,8,15);
			p_mvchoose (player, &yloc, &xloc, "Aim where?", NULL, &show_path_on_overlay);
			if (yloc == -1)
			{
				gra_show (sc_status);
				return 0;
			}
			sk_water_bolt (player, yloc, xloc, sk);
			return 1;
		case SK_FROST:
			gra_hide (sc_status);
			p_mvchoose (player, &yloc, &xloc, "Aim where?", NULL, &show_disc_on_overlay);
			if (yloc == -1)
			{
				gra_show (sc_status);
				return 0;
			}
			sk_frost (player, yloc, xloc, sk);
			return 1;
		case SK_FLAMES:
			sk_flames (player);
			return 1;
		case SK_SCRY:
			sk_scry (player, sk);
			return 1;
		case SK_USE_MARTIAL_ARTS:
		case SK_USE_LONGSWORD:
		case SK_USE_AXE:
		case SK_USE_HAMMER:
		case SK_USE_DAGGER:
		case SK_USE_SHORTSWORD:
			break;
		case SK_NUM:
			break;
		}
	}
	return 0;
}

Graph overlay = NULL;

void show_disc_on_overlay (enum P_MV action, int fy, int fx, int ty, int tx)
{
	if (action == P_MV_END)
	{
		gra_hide (overlay);
		return;
	}
	else if (action == P_MV_START)
	{
		if (!overlay)
			overlay = gra_init (map_graph->h, map_graph->w, 0, 0, map_graph->vh, map_graph->vw);
		gra_show (overlay);
		gra_movecam (overlay, map_graph->cy, map_graph->cx);
	}
	gra_clear (overlay);
	gra_drawdisc (overlay, ty, tx, 3, ' ');
	//gra_mvaddch (overlay, fy, fx, 0);
}

void show_path_on_overlay (enum P_MV action, int fy, int fx, int ty, int tx)
{
	if (action == P_MV_END)
	{
		gra_hide (overlay);
		return;
	}
	else if (action == P_MV_START)
	{
		if (!overlay)
			overlay = gra_init (map_graph->h, map_graph->w, 0, 0, map_graph->vh, map_graph->vw);
		gra_show (overlay);
		gra_movecam (overlay, map_graph->cy, map_graph->cx);
	}
	gra_clear (overlay);
	gra_drawline (overlay, ty, tx, fy, fx, output_colours);
	gra_mvaddch (overlay, fy, fx, 0);
}

Graph gra_n = NULL;
void p_anotify (const char *msg)
{
	if (gra_n)
		p_endnotify ();
	else
		gra_n = gra_init (20, 54, 0, 0, 20, 54);
	Vector fmt = p_formatted (msg, 50);
	int h = fmt->len + 2, w = 50 + 4;
	gra_fbox (gra_n, 0, 0, h-1, w-1, ' ');
	int i;
	for (i = 0; i < fmt->len; ++ i)
	{
		struct MenuOption *m = v_at (fmt, i);
		if (m->ex_str)
			gra_mvaprintex (gra_n, 1+i, 2, m->ex_str);
	}
	p_ffree (fmt);
}

void p_notify (const char *msg, ...)
{
	va_list args;
	char out[1024];

	va_start (args, msg);
	vsnprintf (out, 1024, msg, args);
	va_end (args);

	p_anotify (out);
}

void p_endnotify ()
{
	if (!gra_n)
		return;
	int w;
	for (w = 0; w < gra_n->a; ++ w)
		gra_baddch (gra_n, w, 0);
}

void p_mvchoose (struct Monster *player, int *yloc, int *xloc,
	const char *instruct, const char *confirm, void (*update_output) (enum P_MV, int, int, int, int))
{
	int orig_y = map_graph->csr_y, orig_x = map_graph->csr_x;
	if (update_output)
		update_output (P_MV_START, orig_y, orig_x, orig_y, orig_x);
	if (instruct)
		p_notify (instruct);
	gr_refresh ();
	int xmove, ymove;
	char key = p_getch (player);
	p_move (&ymove, &xmove, key);
	while (key != '.' || (confirm && (p_ask (player, "yn", confirm) != 'y')))
	{
		if (key == CH_ESC)
		{
			if (update_output)
				update_output (P_MV_END, player->yloc, player->xloc, map_graph->csr_y, map_graph->csr_x);
			*yloc = -1;
			gra_cmove (map_graph, orig_y, orig_x);
			p_endnotify ();
			return;
		}
		if (ymove == -1 && map_graph->csr_y > 0 && map_graph->csr_y - map_graph->cy > 0)
			gra_cmove (map_graph, map_graph->csr_y-1, map_graph->csr_x);
		else if (ymove == 1 && map_graph->csr_y < map_graph->h-1 &&
			map_graph->csr_y - map_graph->cy < map_graph->vh-1)
			gra_cmove (map_graph, map_graph->csr_y+1, map_graph->csr_x);
		if (xmove == -1 && map_graph->csr_x > 0 && map_graph->csr_x - map_graph->cx > 0)
			gra_cmove (map_graph, map_graph->csr_y, map_graph->csr_x-1);
		else if (xmove == 1 && map_graph->csr_x < map_graph->w-1 &&
			map_graph->csr_x - map_graph->cx < map_graph->vw-1)
			gra_cmove (map_graph, map_graph->csr_y, map_graph->csr_x+1);
		//if (gra_nearedge (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x))
		//	gra_centcam (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x);
		if (update_output)
			update_output (P_MV_CHOOSING, player->yloc, player->xloc, map_graph->csr_y, map_graph->csr_x);
		key = p_getch (player);
		p_move (&ymove, &xmove, key);
	}
	*yloc = map_graph->csr_y;
	*xloc = map_graph->csr_x;
	gra_cmove (map_graph, orig_y, orig_x);
	if (update_output)
		update_output (P_MV_END, player->yloc, player->xloc, map_graph->csr_y, map_graph->csr_x);
	p_endnotify ();
}

int interp_hex (char in)
{
	if ('0' <= in && in <= '9')
		return in - '0';
	if ('A' <= in && in <= 'F')
		return in + 10 - 'A';
	if ('a' <= in && in <= 'f')
		return in + 10 - 'a';
	return -1;
}

char to_hex (int in)
{
	if (in < 0 || in > 15)
		return 0;
	if (in < 10)
		return '0' + in;
	return in - 10 + 'A';
}

char gl_format_str[9] = {'0', '0', '0', '0', '0', '0', '0', '0', 0};
char *gl_format (glyph gl)
{
	int i;
	for (i = 0; i < 8; ++ i)
		gl_format_str[i] = to_hex ((gl >> (4 * (7-i))) & 0xF);
	return gl_format_str;
}

/* produces output formatted to a given width.
 * special commands are '#' followed by:
 * '#': outputs a '#';
 * 'l', 'c', 'r': left-, centre-, and right-align respectively;
 * 'g': followed by a 8-digit hex repn outputs a given glyph;
 * 'n': changes all subsequent characters to a given colour
 */
Vector p_formatted (const char *input, int max_line_len)
{
	Vector lines = v_dinit (sizeof (struct MenuOption));
	Vector formats = v_dinit (sizeof (struct FormattedGlyph));
	int i;
	glyph cur_gl = 0;
	enum P_FORMAT cur_pos = FMT_LEFT;
	struct FormattedGlyph fg;
	for (i = 0; input[i]; ++ i)
	{
		if (input[i] == '#')
		{
			++ i;
			if (!input[i])
				break;
			if (input[i] == '#')
			{
				fg = (struct FormattedGlyph) {input[i] | (cur_gl&0xFFFFFF00), cur_pos};
				v_push (formats, &fg);
			}
			else if (input[i] == 'l')
				cur_pos = FMT_LEFT;
			else if (input[i] == 'c')
				cur_pos = FMT_CENTRE;
			else if (input[i] == 'r')
				cur_pos = FMT_RIGHT;
			else if (input[i] == 'g')
			{
				int j;
				glyph gl = 0;
				for (j = 1; j <= 8 && input[i+j]; ++ j)
				{
					glyph k = interp_hex (input[i+j]);
					if (k == -1)
						break;
					gl += k << (4*(8-j));
				}
				if (j <= 8)
					break;
				i += 8;
				fg = (struct FormattedGlyph) {gl, cur_pos};
				v_push (formats, &fg);
			}
			else if (input[i] == 'n')
			{
				int j;
				glyph gl = 0;
				for (j = 1; j <= 8 && input[i+j]; ++ j)
				{
					glyph k = interp_hex (input[i+j]);
					if (k == -1)
						break;
					gl += k << (4*(8-j));
				}
				if (j <= 8)
					break;
				i += 8;
				cur_gl = gl;
			}
			else if (input[i] == 'o')
			{
				fg = (struct FormattedGlyph) {0, FMT_MENUOPTION};
				v_push (formats, &fg);
			}
			continue;
		}
		if (input[i] == '\n')
		{
			fg = (struct FormattedGlyph) {'\n', cur_pos};
			v_push (formats, &fg);
			cur_pos = FMT_LEFT;
			continue;
		}
		fg = (struct FormattedGlyph) {input[i] | (cur_gl&0xFFFFFF00), cur_pos};
		v_push (formats, &fg);
	}

	int j, curnum;
	struct MenuOption m = (struct MenuOption) {-1, 0, NULL};
	for (i = 0, j = 0, curnum = 0; i < formats->len; ++ i)
	{
		struct FormattedGlyph *fg = v_at (formats, i);
		if (fg->fmt == FMT_MENUOPTION)
		{
			m = (struct MenuOption) {curnum++, 0, NULL};
			continue;
		}
		if (j == max_line_len || (fg->gl & 0xFF) == '\n')
		{
			if (j < max_line_len && m.ex_str)
				m.ex_str[j] = 0;
			j = 0;
			v_push (lines, &m);
			m = (struct MenuOption) {-1, 0, NULL};
			if ((fg->gl & 0xFF) == '\n')
				continue;
		}
		if (!m.ex_str)
		{
			m.ex_str = malloc(sizeof(glyph) * max_line_len);
			m.len = max_line_len;
		}
		m.ex_str[j] = fg->gl;
		++ j;
	}
	if (j < max_line_len && m.ex_str)
		m.ex_str[j] = 0;
	v_push (lines, &m);
	v_free (formats);

	return lines;

}

void p_ffree (Vector lines)
{
	int i;
	for (i = 0; i < lines->len; ++ i)
	{
		struct MenuOption *mo = v_at (lines, i);
		free (mo->ex_str);
	}
	v_free (lines);
}

int p_flines (const char *input)
{
	Vector lines = p_formatted (input, 50);
	int ret = p_menuex (lines, "(Space)", 50);
	p_ffree (lines);
	return ret;
}

int player_sees_mons (struct Monster *mons)
{
	if (!mons)
		return 1;
	return dlv_lvl(mons->dlevel)->seen[map_buffer (mons->yloc, mons->xloc)] == 2;
}

int player_sees_item (struct Item *item)
{
	switch (item->loc.loc)
	{
		case LOC_NONE:
			panic ("item in LOC_NONE in player_sees_item");
		case LOC_DLVL:
			return dlv_lvl (item->loc.dlvl.dlevel)->
				seen[map_buffer (item->loc.dlvl.yloc, item->loc.dlvl.xloc)] == 2;
		case LOC_INV:
			return player_sees_mons (MTHIID(item->loc.inv.monsID));
		case LOC_WIELDED:
			return player_sees_mons (MTHIID(item->loc.wield.monsID));
		case LOC_FLIGHT:
			return dlv_lvl (item->loc.fl.dlevel)->
				seen[map_buffer (item->loc.fl.yloc, item->loc.fl.xloc)] == 2;
	}
	return 0;
}

void eff_mons_fail_throw (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("You fail to throw the %s.", it_typename (item));
	else
		p_msg ("The %s fails to throw the %s.", mons_typename (mons), it_typename (item));
	return;
}

void eff_item_dissipates (struct Item *item)
{
	if (!player_sees_item (item))
		return;
	p_msg ("The %s dissipates.", it_typename (item));
}

void eff_item_absorbed (struct Item *item)
{
	if (!player_sees_item (item))
		return;
	p_msg ("The %s is absorbed.", it_typename (item));
}

void eff_item_hits_wall (struct Item *item)
{
	if (!player_sees_item (item))
		return;
	p_msg ("The %s hits the wall.", it_typename (item));
}

void eff_proj_misses_mons (struct Item *item, struct Monster *mons)
{
	if (!player_sees_item (item))
		return;
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("The %s misses you!", it_typename (item));
	else
		p_msg ("The %s misses the %s!", it_typename (item), mons_typename (mons));
}

void eff_proj_hits_mons (struct Item *item, struct Monster *mons, int damage)
{
	if (!player_sees_item (item))
		return;
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("The %s hits you for "COL_RED("%d")"!", it_typename (item), damage);
	else
		p_msg ("The %s hits the %s for "COL_RED("%d")"!", it_typename (item), mons_typename (mons), damage);
}

void eff_mons_starts_hit (struct Monster *mons, int y, int x, Tick arrival)
{
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		return;
	if (!player_sees_mons (mons))
		return;
	p_msg ("The %s swings!", mons_typename (mons));
}

void eff_aux_mons_misses_mons (struct Monster *fr, struct Monster *to, const char *adverb)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (to == fr)
	{
		if (mons_isplayer (to))
			p_msg ("You%s miss yourself!", adverb);
		else
			p_msg ("The %s%s misses itself!", mons_typename (to), adverb);
	}
	else if (mons_isplayer (to))
		p_msg ("The %s%s misses you!", mons_typename (fr), adverb);
	else if (mons_isplayer (fr))
		p_msg ("You%s miss the %s!", adverb, mons_typename (to));
	else
		p_msg ("The %s%s misses the %s!", mons_typename (fr), adverb, mons_typename (to));
}

void eff_mons_tiredly_misses_mons (struct Monster *fr, struct Monster *to)
{
	eff_aux_mons_misses_mons (fr, to, " tiredly");
}

void eff_mons_misses_mons (struct Monster *fr, struct Monster *to)
{
	eff_aux_mons_misses_mons (fr, to, "");
}

void eff_mons_just_misses_mons (struct Monster *fr, struct Monster *to)
{
	eff_aux_mons_misses_mons (fr, to, " just");
}

void eff_mons_hits_mons (struct Monster *fr, struct Monster *to, int damage)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (to == fr)
	{
		if (mons_isplayer (to))
			p_msg ("You hit yourself for "COL_RED("%d")"!", damage);
		else
			p_msg ("The %s hits itself!", mons_typename (to));
	}
	else if (mons_isplayer (to))
		p_msg ("The %s hits you for "COL_RED("%d")"!", mons_typename (fr), damage);
	else if (mons_isplayer (fr))
		p_msg ("You hit the %s for "COL_RED("%d")"!", mons_typename (to), damage);
	else
		p_msg ("The %s hits the %s for "COL_RED("%d")"!", mons_typename (fr), mons_typename (to), damage);
}

void eff_mons_bleeds (struct Monster *mons, int damage)
{
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("You bleed for "COL_RED("%d")"!", damage);
	else
		p_msg ("The %s bleeds!", mons_typename (mons));
}

void eff_mons_burns (struct Monster *mons, int damage)
{
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("You are burned for "COL_RED("%d")"!", damage);
	else
		p_msg ("The %s is burned!", mons_typename (mons));
}

void eff_mons_kills_mons (struct Monster *fr, struct Monster *to)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (fr == to)
	{
		if (mons_isplayer (to))
			p_msg ("You kill yourself!");
		else
			p_msg ("The %s kills itself!", mons_typename (to));
	}
	else if (mons_isplayer (to))
		p_msg ("The %s "COL_RED("kills")" you!", mons_typename (fr));
	else if (mons_isplayer (fr))
		p_msg ("You "COL_RED("kill")" the %s!", mons_typename (to));
	else
		p_msg ("The %s "COL_RED("kills")" the %s!", mons_typename (fr), mons_typename (to));
}

void eff_mons_sk_levels_up (struct Monster *mons, Skill sk)
{
	if (!mons_isplayer (mons))
		return;
	p_msg ("Congratulations! Your %s is now level %d!", sk_name (sk), sk->level);
}

void eff_mons_levels_up (struct Monster *mons)
{
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg (COL_BRIGHT("Level up!")" You are now level "COL_YELLOW("%d")".", mons->level);
	else
		p_msg ("The %d seems more experienced.", mons_typename (mons));
}

static char msg[IT_DESC_LEN];
void eff_mons_picks_up_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	int p = mons_isplayer (mons);
	it_desc (msg, item, p ? mons : NULL);
	if (p)
		p_msg ("%s", msg);
	else
		p_msg ("The %s picks up %s.", mons_typename (mons), msg);
}

void eff_mons_wields_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	int p = mons_isplayer (mons);
	it_desc (msg, item, p ? mons : NULL);
	if (p)
		p_msg ("You wield %s.", msg);
	else
		p_msg ("The %s wields %s.", mons_typename (mons), msg);
}

void eff_mons_unwields (struct Monster *mons)
{
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("You are now empty-handed.");
	else
		p_msg ("The %s is now empty-handed.", mons_typename (mons));
}

void eff_mons_wears_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	int p = mons_isplayer (mons);
	it_desc (msg, item, p ? mons : NULL);
	if (p)
		p_msg ("You wear %s.", msg);
	else
		p_msg ("The %s wears %s.", mons_typename (mons), msg);
}

void eff_mons_takes_off_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	int p = mons_isplayer (mons);
	it_desc (msg, item, p ? mons : NULL);
	if (p)
		p_msg ("You take off %s.", msg);
	else
		p_msg ("The %s takes off %s.", mons_typename (mons), msg);
}

void eff_mons_angers_mons (struct Monster *fr, struct Monster *to)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (mons_isplayer (fr))
		p_msg ("You anger the %s!", mons_typename (to));
	else
		p_msg ("The %s angers the %s.", mons_typename (fr), mons_typename (to));
}

void eff_mons_calms (struct Monster *mons)
{
	if (!player_sees_mons (mons))
		return;
	p_msg ("The %s calms.", mons_typename (mons));
}

