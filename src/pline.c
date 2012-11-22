/* pline.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/pline.h"
#include "include/loop.h"
#include "include/graphics.h"

#include <stdio.h>
#include <malloc.h>

void panel (int height)
{
	gr_mode (TMODE);
	int i, j;
	for (i = 0; i < glnumx; ++ i)
	{
		gr_mvaddch (glnumy - height, i, COL_TXT_BRIGHT | ACS_HLINE);
		for (j = glnumy + 1 - height; j < glnumy; ++ j)
		{
			gr_mvaddch (j, i, ' ');
		}
	}
	gr_refresh ();
	gr_getch ();
	gr_mode (GMODE);
}

int msg_size_pline = 0, line_pline = 0;

bool plined = false;
void aline (const char *, bool);

char pask (const char *in, const char *out, ...)
{
	va_list args;
	char c;
	char o[80];
	int where = 0;
	plined = true;

	/* format the string */
	va_start (args, out);
	vsprintf (o, out, args);
	va_end (args);

	/* make the question */
	where += strlen(o);
	memcpy (o + where, " (", 2);
	where += 2;
	memcpy (o + where, in, strlen(in));
	where += strlen(in);
	memcpy (o + where, ")", 1);
	where += 1;
	o[where] = '\0'; /* terminating null character */

	/* print the question */
	aline (o, 1);

	/* wait for answer */
	do
		c = gr_getch ();
	while (strchr (in, c) == NULL && c != ' ' && c != 0x1B);
	return c;
}

char pline_history[20][256] = { {0,}, };

int pline_where = 0, pline_pretend = -1;

void pline_get_his ()
{
	if (pline_pretend == -1)
		pline_pretend = pline_where - 1;
	if (pline_pretend == -1)
		pline_pretend = 19;
	aline (pline_history[pline_pretend], 0);
}

void pline (const char *out, ...)
{
	va_list args;
	char actual[500];
	plined = true;

	va_start (args, out);
	vsprintf (actual, out, args);
	aline (actual, true);
	va_end (args);
}

void aline_col (uint32_t col, const char *out, bool historicise)
{
	int len;
	plined = true;
	pline_pretend = -1;
	if (historicise)
	{
		strcpy (pline_history[pline_where++], out);
		if (pline_where == 20)
			pline_where = 0;
	}
	// TODO set_col_attr(col);
	if (strlen (out) > glnumx - 5)
	{
		/* TODO change */
		out = "pline length exceeded";
	}

	gr_mode (TMODE);
	gr_setline (line_pline, ' ');
	len = strlen (out);
	if (msg_size_pline + len >= glnumx - 9)
	{
		gr_mvprintc (line_pline, msg_size_pline, "--more--");
		gr_move (line_pline, msg_size_pline + 9);
		msg_size_pline = 0;
		gr_getch ();
	}
	if (msg_size_pline == 0)
		line_reset ();

	gr_mvprintc (line_pline, msg_size_pline, "%s ", out);
	msg_size_pline += len + 1;
	gr_move (line_pline, msg_size_pline);
	gr_refresh ();
	gr_mode (GMODE | NOREF);
}

void pline_col (uint32_t col, const char *out, ...)
{
	va_list args;
	char actual[500];
	plined = true;

	va_start (args, out);
	vsprintf (actual, out, args);
	aline_col (col, actual, true);
	va_end (args);
}

void aline (const char *out, bool historicise)
{
	aline_col (COL_TXT_DEF, out, historicise);
}

void line_reset ()
{
	int i;

	for (i = 0; i < glnumx; ++i)
		gr_mvforce (line_pline, i);
	msg_size_pline = 0;
}

bool pline_check ()
{
	bool ret = plined;
	plined = 0;
	return ret;
}

void mlines (int num, ...)
{
	va_list args;
	Vector lines = v_init (sizeof(char*), num + 1);

	va_start (args, num);

	do
	{
		char *str = va_arg (args, char *);
		v_push (lines, &str);
	}
	while (--num);

	va_end (args);

	mlines_vec (lines);
	v_free (lines);
}

void mlines_vec (Vector lines)
{
	int i;
	int l_no;

	if (lines->len <= 0)
		return;
	else if (lines->len == 1)
		aline (v_at (lines, 0), true);
	else
	{
		gr_mode (TMODE);
		gr_clear ();
		for (l_no = 0, i = 0; i < lines->len; ++l_no, ++ i)
		{
			gr_mvprintc (l_no, 0, "%s", v_at(lines, i));
			if (l_no == glnumy - 2)
			{
				gr_mvprintc (glnumy - 1, 0, "--more--");
				gr_getch ();
				l_no = -1;
			}
		}
		gr_mvprintc (glnumy - 1, 0, "--END--");
		gr_move (glnumy - 1, 7);
		gr_refresh ();
	}
	gr_getch ();
	gr_mode (GMODE);
}

void mask_vec (int n, Vector ret, Vector things)
{
	/* TODO ask the player; delete loop */
	int i;
	for (i = 0; i < things->len; ++ i)
		v_push (ret, v_at (things, i));
}
