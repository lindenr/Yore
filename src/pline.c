/* pline.c */

#include "include/all.h"
#include "include/pline.h"
#include "include/loop.h"
#include "include/util.h"
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
	while ((!is_in (in, c)) && c != ' ' && c != 0x1B);
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

void mlines (int num_lines, ...)
{
	va_list args;
	struct List list = LIST_INIT;
	int num = num_lines;

	va_start (args, num_lines);

	do
		push_back (&list, va_arg (args, char *));
	while (--num);

	va_end (args);

	mlines_list (list, num_lines);
	list_free (&list);
}

void mlines_list (struct List list, int num_lines)
{
	struct list_iter *i;
	int l_no;

	screenshot();
	if (num_lines <= 0)
		return;
	else if (num_lines == 1)
		aline (list.beg->data, true);
	else
	{
		gr_mode (TMODE);
		gr_clear ();
		for (l_no = 0, i = list.beg; iter_good(i); ++l_no, next_iter(&i))
		{
			gr_mvprintc (l_no, 0, "%s", i->data);
			if (l_no == glnumy - 2)
			{
				gr_mvprintc (l_no + 1, 0, "--more--");
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

void mask_list (struct List *ret, struct List things)
{
	struct List piles = LIST_INIT;

	/* Divide up into piles */
	item_piles (&piles, &things);

	/* TODO ask the player; delete loop */
	struct list_iter *li;
	for (li = things.beg; iter_good(li); next_iter(&li))
	{
		push_back(ret, li->data);
	}

	/* Empty piles */
	for (li = piles.beg; iter_good(li); next_iter(&li))
	{
		free(li->data);
		if (li != piles.beg)
			free(li->prev);
	}
	free (li->prev);
}
