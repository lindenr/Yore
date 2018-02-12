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
			gra_mvprint (gpan, 8, 3, "Exp     %d (%d)", player->exp, exp_needed);
		else
			gra_mvprint (gpan, 8, 3, "Exp     %d (%c)", player->exp, 0xEC);
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
	p_menu_draw (box, lines, curchoice);
	if (curchoice == -1)
	{
		ret = gr_getch ();
		gra_free (box);
		return ret;
	}
	do
	{
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
		p_menu_draw (box, lines, curchoice);
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

glyph output_colours;
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
			if (pskills->len) for (j = 1; j < w-1; ++ j)
				gra_invert (sc_skills, 4+selected, j);
			Skill sk = v_at (pskills, selected);
			p_msgbox (sk_desc(sk));
			if (pskills->len) for (j = 1; j < w-1; ++ j)
				gra_invert (sc_skills, 4+selected, j);
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
				output_colours = COL_BG(0,0,0) | COL_TXT(15,15,15);
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
				output_colours = COL_BG(11,0,0) | COL_TXT(15,15,0);
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
				output_colours = COL_BG(0,0,8) | COL_TXT(8,8,15);
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
	gra_drawline (overlay, ty, tx, fy, fx, output_colours);
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

/// EXAMPLE inputs
// "#cMENU\n\n#oa#gFF000024 24 gold pieces"
// "#nFF000000 Hello"
char p_formatted (const char *input)
{
	Vector lines = v_dinit (sizeof (struct MenuOption));
	Vector formats = v_dinit (sizeof (struct FormattedGlyph));
	int i;
	glyph cur_gl = 0;
	enum P_FORMAT cur_pos;
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
			else if (input[i] == 'c')
				cur_pos = FMT_CENTRE;
			else if (input[i] == 'g')
			{
				int j;
				glyph gl = 0;
				for (j = 1; j <= 8 && input[i+j]; ++ j)
				{
					int k = interp_hex (input[i+j]);
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
					int k = interp_hex (input[i+j]);
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
				++ i;
				if (!input[i])
					break;
				fg = (struct FormattedGlyph) {input[i], FMT_MENUOPTION};
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

	int j;
	struct MenuOption m = (struct MenuOption) {0, {0,}, NULL};
	for (i = 0, j = 0; i < formats->len; ++ i)
	{
		struct FormattedGlyph *fg = v_at (formats, i);
		if (fg->fmt == FMT_MENUOPTION)
		{
			m = (struct MenuOption) {(char) fg->gl, {0,}, NULL};
			continue;
		}
		if (j == 40 || (fg->gl & 0xFF) == '\n')
		{
			j = 0;
			v_push (lines, &m);
			m = (struct MenuOption) {0, {0,}, NULL};
			if ((fg->gl & 0xFF) == '\n')
				continue;
		}
		m.ex_str[j] = fg->gl;
		++ j;
	}
	v_push (lines, &m);

	char ret = p_menuex (lines);
	v_free (lines);
	return ret;
}

int player_sees_mons (struct Monster *mons)
{
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
			return mons_isplayer (MTHIID(item->loc.inv.monsID));
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
		p_msg ("You fail to throw the %s.", item->type.name);
	else
		p_msg ("The %s fails to throw the %s.", mons->mname, item->type.name);
	return;
}

void eff_item_dissipates (struct Item *item)
{
	if (!player_sees_item (item))
		return;
	p_msg ("The %s dissipates.", item->type.name);
}

void eff_item_absorbed (struct Item *item)
{
	if (!player_sees_item (item))
		return;
	p_msg ("The %s is absorbed.", item->type.name);
}

void eff_item_hits_wall (struct Item *item)
{
	if (!player_sees_item (item))
		return;
	p_msg ("The %s hits the wall.", item->type.name);
}

void eff_proj_misses_mons (struct Item *item, struct Monster *mons)
{
	if (!player_sees_item (item))
		return;
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("The %s misses you!", item->type.name);
	else
		p_msg ("The %s misses the %s!", item->type.name, mons->mname);
}

void eff_proj_hits_mons (struct Item *item, struct Monster *mons, int damage)
{
	if (!player_sees_item (item))
		return;
	if (!player_sees_mons (mons))
		return;
	if (mons_isplayer (mons))
		p_msg ("The %s hits you for %d!", item->type.name, damage);
	else
		p_msg ("The %s hits the %s for %d!", item->type.name, mons->mname, damage);
}

void eff_mons_tiredly_misses_mons (struct Monster *fr, struct Monster *to)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (mons_isplayer (to))
		p_msg ("The %s tiredly misses you!", fr->mname);
	else if (mons_isplayer (fr))
		p_msg ("You tiredly miss the %s!", to->mname);
	else
		p_msg ("The %s tiredly misses the %s!", fr->mname, to->mname);
}

void eff_mons_misses_mons (struct Monster *fr, struct Monster *to)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (mons_isplayer (to))
		p_msg ("The %s misses you!", fr->mname);
	else if (mons_isplayer (fr))
		p_msg ("You miss the %s!", to->mname);
	else
		p_msg ("The %s misses the %s!", fr->mname, to->mname);
}

void eff_mons_just_misses_mons (struct Monster *fr, struct Monster *to)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (mons_isplayer (to))
		p_msg ("The %s just misses you!", fr->mname);
	else if (mons_isplayer (fr))
		p_msg ("You just miss the %s!", to->mname);
	else
		p_msg ("The %s just misses the %s!", fr->mname, to->mname);
}

void eff_mons_hits_mons (struct Monster *fr, struct Monster *to, int damage)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (mons_isplayer (to))
		p_msg ("The %s hits you for %d!", fr->mname, damage);
	else if (mons_isplayer (fr))
		p_msg ("You hit the %s for %d!", to->mname, damage);
	else
		p_msg ("The %s hits the %s for %d!", fr->mname, to->mname, damage);
}

void eff_mons_kills_mons (struct Monster *fr, struct Monster *to)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (mons_isplayer (to))
		p_msg ("The %s kills you!", fr->mname);
	else if (mons_isplayer (fr))
		p_msg ("You kill the %s!", to->mname);
	else
		p_msg ("The %s kills the %s!", fr->mname, to->mname);
}

void eff_mons_picks_up_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	char *msg = get_inv_line (item);
	if (mons_isplayer (mons))
		p_msg ("%s", msg);
	else
		p_msg ("The %s picks up %s.", mons->mname, msg);
	free (msg);
}

void eff_mons_wields_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	char *msg = get_inv_line (item);
	if (mons_isplayer (mons))
		p_msg ("You wield %s.", msg);
	else
		p_msg ("The %s wields %s.", mons->mname, msg);
	free (msg);
}

void eff_mons_wears_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	char *msg = get_inv_line (item);
	if (mons_isplayer (mons))
		p_msg ("You wear %s.", msg);
	else
		p_msg ("The %s wears %s.", mons->mname, msg);
	free (msg);
}

void eff_mons_takes_off_item (struct Monster *mons, struct Item *item)
{
	if (!player_sees_mons (mons))
		return;
	if (!player_sees_item (item))
		return;
	char *msg = get_inv_line (item);
	if (mons_isplayer (mons))
		p_msg ("You take off %s.", msg);
	else
		p_msg ("The %s takes off %s.", mons->mname, msg);
	free (msg);
}

void eff_mons_angers_mons (struct Monster *fr, struct Monster *to)
{
	if (!player_sees_mons (fr))
		return;
	if (!player_sees_mons (to))
		return;
	if (mons_isplayer (fr))
		p_msg ("You anger the %s!", to->mname);
	else
		p_msg ("The %s angers the %s.", fr->mname, to->mname);
}

void eff_mons_calms (struct Monster *mons)
{
	if (!player_sees_mons (mons))
		return;
	p_msg ("The %s calms.", mons->mname);
}

