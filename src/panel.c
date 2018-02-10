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

#include <stdlib.h>
#include <stdarg.h>

int p_height, p_width;

Vector messages = NULL;

Graph gpan = NULL;

void p_pane (struct Monster *player)
{
	int i;
	int xpan = 0, ypan = gr_h - PANE_H;
	if (!gpan)
	{
		gpan = gra_init (p_height, p_width, ypan, xpan, p_height, p_width);
		gpan->def = COL_PANEL;
	}
	gra_fbox (gpan, 0, 0, p_height-1, p_width-1, ' ');

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
	gra_mvaddch (gpan, 1, 1, 0xAF | COL_TXT(15,7,0));

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
			gra_mvprint (gpan, 8, 3, "XP      %d (%d)", player->exp, exp_needed);
		else
			gra_mvprint (gpan, 8, 3, "XP      %d", player->exp);
		gra_mvaddch (gpan, 2, 1, '@' | COL_TXT(15,15,15));
		gra_mvaddch (gpan, 3, 1, 3 | COL_TXT_RED(15));
		gra_mvaddch (gpan, 4, 1, 5 | COL_TXT_GREEN(15));
		gra_mvaddch (gpan, 5, 1, 4 | COL_TXT(0,5,15));
		gra_mvaddch (gpan, 6, 1, '[' | COL_TXT(11,11,0));
		gra_mvaddch (gpan, 7, 1, 6 | COL_TXT(7,15,15));
		gra_mvaddch (gpan, 8, 1, 0xE4 | COL_TXT(15,0,15));
		gra_mvprint (gpan, 2, 100, "SHIELD Y:%d X:%d", player->status.defending.ydir, player->status.defending.xdir);
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
		int len = strlen (msg->msg);
		gra_mvprint (gpan, 1 + i, (gr_w - len)/2, msg->msg);
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
	v_pstr (lines, "#[OK]");
	p_lines (lines);
	v_free (lines);
}

void p_amsg (const char *str)
{
	struct P_msg msg;
	msg.expiry = 0;
	strncpy (msg.msg, str, P_MSG_LEN-1);
	msg.msg[P_MSG_LEN-1] = 0;
	v_push (messages, &msg);
}

void p_menu_draw (Graph box, Vector lines, int curchoice)
{
	int i;
	int h = box->h, w = box->w;
	for (i = 0; i < lines->len; ++ i)
	{
		struct MenuOption *line = v_at (lines, i);
		if (line->letter)
		{
			gra_mvaddch (box, i+1, 2, ACS_PILLAR);
			gra_mvaddch (box, i+1, 3, ' ');
			gra_mvaddch (box, i+1, 4, line->letter);
		}
		gra_mvaprintex (box, i+1, 6, line->ex_str);
	}
	gra_box (box, 0, 0, h-1, w-1);
	if (curchoice != -1)
	{
		gra_invert (box, curchoice+1, 2);
		gra_invert (box, curchoice+1, 3);
		gra_invert (box, curchoice+1, 4);
	}
}

char p_menuex (Vector lines)
{
	int i, curchoice = -1;
	int max = 44;

	int h = lines->len + 2, w = max+4;
	int yloc = (gr_h - h)/2, xloc = (gr_w - w)/2;

	Graph box = gra_init (h, w, yloc, xloc, h, w);
	gra_fbox (box, 0, 0, h-1, w-1, ' ');

	for (i = 0; i < lines->len; ++ i)
	{
		struct MenuOption *line = v_at (lines, i);
		if (line->letter)
		{
			curchoice = i;
			break;
		}
	}

	char ret;
	if (curchoice == -1)
	{
		ret = gr_getch ();
		gra_free (box);
		return ret;
	}
	do
	{
		p_menu_draw (box, lines, curchoice);
		ret = gr_getch ();
		if (ret == CH_ESC || ret == ' ' ||
			ret == '-')
			break;
		for (i = 0; i < lines->len; ++ i)
		{
			struct MenuOption *line = v_at (lines, i);
			if (line->letter == ret)
				break;
		}
		if (i < lines->len)
			break;
		if (ret == '\n')
		{
			gra_free (box);
			struct MenuOption *line = v_at (lines, curchoice);
			return line->letter;
		}
		else if (ret == GRK_UP)
		{
			for (i = curchoice - 1; i >= 0; -- i)
			{
				struct MenuOption *line = v_at (lines, i);
				if (line->letter)
					break;
			}
			if (i >= 0)
				curchoice = i;
		}
		else if (ret == GRK_DN)
		{
			for (i = curchoice + 1; i < lines->len; ++ i)
			{
				struct MenuOption *line = v_at (lines, i);
				if (line->letter)
					break;
			}
			if (i < lines->len)
				curchoice = i;
		}
	}
	while (1);

	gra_free (box);

	return ret;
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

char p_ask (struct Monster *player, const char *results, const char *question)
{
	p_amsg (question);
	p_pane (player);
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
			gra_mvaprint (box, i+1, 2, str);
		else
			gra_cprint (box, i+1, str+1);
	}
	gra_box (box, 0, 0, h-1, w-1);
	char ret = gr_getch();

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
	gra_cprint (sc_status, 4, "Name: %12s  %c  Race:  %s       ", player->name, ACS_VLINE, player->mname);
	gra_cprint (sc_status, 5, "  HP: %d/%d  %c  ST:    %d/%d",
				player->HP, player->HP_max, ACS_VLINE, player->ST, player->ST_max);
	gra_cprint (sc_status, 6, "LV %d:%d/infinity   %c  Speed: %d       ", player->level, player->exp, ACS_VLINE, player->speed);
	gra_cprint (sc_status, 7, "Str: %d", player->str);
	gra_cprint (sc_status, 8, "Con: %d", player->con);
	gra_cprint (sc_status, 9, "Agi: %d", player->agi);
	gra_cprint (sc_status, 11, "Press 'S' to save.");

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
		char in = gr_getch ();
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

/* Skills screen */
Graph sc_skills = NULL;
int p_skills (struct Monster *player, enum PanelType type)
{
	Vector pskills = player->skills;
	int h = 6 + pskills->len, w = 41;
	int y = (gr_h - h)/2, x = (gr_w - w)/2;

	sc_skills = gra_init (h, w, y, x, h, w);
	sc_skills->def = COL_SKILLS;

	gra_fbox (sc_skills, 0, 0, h-1, w-1, ' ');
	gra_mvprint (sc_skills, 0, w-7, "(Space)");
	gra_cprint (sc_skills, 2, "SKILLS");
	if (pskills->len == 0)
		gra_cprint (sc_skills, 3, "(no skills available)");
	else
		gra_cprint (sc_skills, 3, "(enter to inspect; letter to use)");

	int i, selected = 0;
	int j;
	while (1)
	{
		char letter = 'a';
		for (i = 0; i < pskills->len; ++ i)
		{
			Skill sk = v_at (pskills, i);
			int row = 4+i;
			if (sk_isact (sk))
				gra_cprint (sc_skills, row, "%c - %s %d:%d", letter, sk_name(sk), sk->level, sk->exp);
			else
				gra_cprint (sc_skills, row, "%s %d:%d", sk_name(sk), sk->level, sk->exp);
			++ letter;
		}
		if (pskills->len) for (j = 1; j < w-1; ++ j)
			gra_invert (sc_skills, 4+selected, j);
		char in = gr_getch();
		if (pskills->len) for (j = 1; j < w-1; ++ j)
			gra_invert (sc_skills, 4+selected, j);
		if (in == CH_ESC)
		{
			gra_free (sc_skills);
			return -1;
		}
		else if (in == ' ')
		{
			gra_free (sc_skills);
			return 0;
		}
		else if (pskills->len == 0)
			continue;
		else if (in == GRK_UP && selected > 0)
			-- selected;
		else if (in == GRK_DN && selected < pskills->len-1)
			++ selected;
		else if (in == CH_LF || in == CH_CR)
		{
			Skill sk = v_at (pskills, selected);
			Vector lines = w_lines (sk_desc(sk), MAX_BOX_LENGTH);
			p_lines (lines);
			v_free (lines);
		}
		else if (in >= 'a' && in < letter)
		{
			int n = in - 'a';
			int yloc, xloc;
			Skill sk = v_at (pskills, n);
			switch (sk->type)
			{
			case SK_NONE:
				break;
			case SK_CHARGE:
				gra_hide (sc_status);
				gra_hide (sc_skills);
				if (player->status.charging)
				{
					gra_free (sc_skills);
					sk_charge (player, 0, 0, v_at (pskills, selected));
					return 1;
				}
				p_mvchoose (player, &yloc, &xloc, "Charge where?", NULL, &show_path_on_overlay);
				if (yloc == -1)
				{
					gra_show (sc_status);
					gra_show (sc_skills);
					continue;
				}
				gra_free (sc_skills);
				sk_charge (player, yloc, xloc, v_at (pskills, selected));
				return 1;
			case SK_DODGE:
				break;
			case SK_FIREBALL:
				if (player->MP < 5)
				{
					p_msgbox ("Not enough MP.");
					continue;
				}
				gra_hide (sc_status);
				gra_hide (sc_skills);
				p_mvchoose (player, &yloc, &xloc, "Aim where?", NULL, &show_path_on_overlay);
				if (yloc == -1)
				{
					gra_show (sc_status);
					gra_show (sc_skills);
					continue;
				}
				gra_free (sc_skills);
				sk_fireball (player, yloc, xloc, v_at (pskills, selected));
				return 1;
			case SK_WATER_BOLT:
				if (player->MP < 6)
				{
					p_msgbox ("Not enough MP.");
					continue;
				}
				gra_hide (sc_status);
				gra_hide (sc_skills);
				p_mvchoose (player, &yloc, &xloc, "Aim where?", NULL, &show_path_on_overlay);
				if (yloc == -1)
				{
					gra_show (sc_status);
					gra_show (sc_skills);
					continue;
				}
				gra_free (sc_skills);
				sk_water_bolt (player, yloc, xloc, v_at (pskills, selected));
				return 1;
			case SK_FROST:
				gra_hide (sc_status);
				gra_hide (sc_skills);
				p_mvchoose (player, &yloc, &xloc, "Aim where?", NULL, &show_disc_on_overlay);
				if (yloc == -1)
				{
					gra_show (sc_status);
					gra_show (sc_skills);
					continue;
				}
				gra_free (sc_skills);
				sk_frost (player, yloc, xloc, v_at (pskills, selected));
				return 1;
			case SK_FLAMES:
				gra_free (sc_skills);
				sk_flames (player);
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
	}
	gra_free (sc_skills);
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
	gra_drawline (overlay, ty, tx, fy, fx, ' '|COL_BG_RED(8));
	gra_mvaddch (overlay, fy, fx, 0);
}

void p_mvchoose (struct Monster *player, int *yloc, int *xloc,
	const char *instruct, const char *confirm, void (*update_output) (enum P_MV, int, int, int, int))
{
	int orig_y = map_graph->csr_y, orig_x = map_graph->csr_x;
	if (instruct)
		p_msg (instruct);
	p_pane (player);
	if (update_output)
		update_output (P_MV_START, orig_y, orig_x, orig_y, orig_x);
	gr_refresh ();
	int xmove, ymove;
	char key = gr_getch ();
	p_move (&ymove, &xmove, key);
	while (key != '.' || (confirm && (p_ask (player, "yn", confirm) != 'y')))
	{
		if (key == CH_ESC)
		{
			if (update_output)
				update_output (P_MV_END, player->yloc, player->xloc, map_graph->csr_y, map_graph->csr_x);
			*yloc = -1;
			gra_cmove (map_graph, orig_y, orig_x);
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
		key = gr_getch ();
		p_move (&ymove, &xmove, key);
	}
	*yloc = map_graph->csr_y;
	*xloc = map_graph->csr_x;
	gra_cmove (map_graph, orig_y, orig_x);
	if (update_output)
		update_output (P_MV_END, player->yloc, player->xloc, map_graph->csr_y, map_graph->csr_x);
}

