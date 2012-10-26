/* pline.c Linden Ralph */

#include "include/all.h"
#include "include/pline.h"
#include "include/loop.h"
#include "include/util.h"
#include "include/graphics.h"

#include <stdio.h>
#include <malloc.h>

int msg_size_pline = 0, line_pline = 0;

bool plined = false;
void aline(const char *, bool);

char pask(const char *in, const char *out, ...)
{
	va_list args;
	char c;
	char o[80];
	int where = 0;
	plined = true;

	/* format the string */
	va_start(args, out);
	vsprintf(o, out, args);
	va_end(args);

	/* make the question */
	where += strlen(o);
	memcpy(o + where, " (", 2);
	where += 2;
	memcpy(o + where, in, strlen(in));
	where += strlen(in);
	memcpy(o + where, ")", 1);
	where += 1;
	o[where] = '\0';			/* terminating null character */

	/* print the question */
	aline(o, 1);

	/* wait for answer */
	do
		c = gr_getch();
	while ((!is_in(in, c)) && c != ' ' && c != 0x1B);
	return c;
}

/* unused */
void mvline(uint32_t yloc, uint32_t xloc, const char *txt, ...)
{
	char out[30];
	va_list args;
	int i, len;

	va_start(args, txt);
	vsprintf(out, txt, args);
	va_end(args);
	len = strlen(out);

	if (!(yloc > 0 && yloc < 79 && xloc > 0 && xloc < 79))
	{
		/* not middle */
		return;
	}
	mvaddch(yloc - 1, xloc - 1, ACS_ULCORNER);
	mvaddch(yloc + 1, xloc - 1, ACS_LLCORNER);
	mvaddch(yloc - 1, xloc, ACS_HLINE);
	mvaddch(yloc + 1, xloc, ACS_HLINE);
	mvaddch(yloc - 1, xloc + 1, ACS_URCORNER);
	mvaddch(yloc + 1, xloc + 1, ACS_LRCORNER);
	if (xloc < 40)
	{
		/* text on RHS */
		mvaddch(yloc, xloc - 1, ACS_VLINE);
		mvaddch(yloc, ++xloc, ACS_LTEE);
		while (xloc < 40)
			mvaddch(yloc, ++xloc, ACS_HLINE);
		++xloc;
		mvaddch(yloc, xloc, ACS_RTEE);
		mvprintw(yloc, xloc + 1, out);
		mvaddch(yloc - 1, xloc, ACS_ULCORNER);
		mvaddch(yloc + 1, xloc, ACS_LLCORNER);
		for (i = 0; i < len; ++i)
		{
			mvaddch(yloc - 1, xloc + i + 1, ACS_HLINE);
			mvaddch(yloc + 1, xloc + i + 1, ACS_HLINE);
		}
		mvaddch(yloc - 1, xloc + len + 1, ACS_URCORNER);
		mvaddch(yloc, xloc + len + 1, ACS_VLINE);
		mvaddch(yloc + 1, xloc + len + 1, ACS_LRCORNER);
	}
	else
	{
		/* text on LHS */
		mvaddch(yloc, xloc + 1, ACS_VLINE);
		mvaddch(yloc, --xloc, ACS_RTEE);
		while (xloc >= 40)
			mvaddch(yloc, --xloc, ACS_HLINE);
		--xloc;
		mvaddch(yloc, xloc, ACS_LTEE);
		mvprintw(yloc, xloc - len, out);
		mvaddch(yloc - 1, xloc, ACS_URCORNER);
		mvaddch(yloc + 1, xloc, ACS_LRCORNER);
		for (i = 0; i < len; ++i)
		{
			mvaddch(yloc - 1, xloc - i - 1, ACS_HLINE);
			mvaddch(yloc + 1, xloc - i - 1, ACS_HLINE);
		}
		mvaddch(yloc - 1, xloc - len - 1, ACS_ULCORNER);
		mvaddch(yloc, xloc - len - 1, ACS_VLINE);
		mvaddch(yloc + 1, xloc - len - 1, ACS_LLCORNER);
	}
	move(0, 0);
}

char pline_history[20][256] = { {0,}, };

int pline_where = 0, pline_pretend = -1;

void pline_get_his()
{
	if (pline_pretend == -1)
		pline_pretend = pline_where - 1;
	if (pline_pretend == -1)
		pline_pretend = 19;
	aline(pline_history[pline_pretend], 0);
}

void pline(const char *out, ...)
{
	va_list args;
	char actual[500];
	plined = true;

	va_start(args, out);
	vsprintf(actual, out, args);
	aline(actual, true);
	va_end(args);
}

void aline_col(uint32_t col, const char *out, bool historicise)
{
	int len;
	plined = true;
	pline_pretend = -1;
	if (historicise)
	{
		strcpy(pline_history[pline_where++], out);
		if (pline_where == 20)
			pline_where = 0;
	}
	set_col_attr(col);
	if (strlen(out) > glnumx - 5)
	{
		/* TODO change */
		out = "pline length exceeded";
	}

	len = strlen(out);
	if (msg_size_pline + len >= glnumx - 9)
	{
		gr_mvprintc(line_pline, msg_size_pline, "--more--");
		gr_move(line_pline, msg_size_pline + 9);
		msg_size_pline = 0;
		gr_getch();
	}
	if (msg_size_pline == 0)
		line_reset ();

	gr_mvprintc(line_pline, msg_size_pline, "%s ", out);
	msg_size_pline += len + 1;
	gr_move(line_pline, msg_size_pline);
	gr_refresh();
}

void pline_col(uint32_t col, const char *out, ...)
{
	va_list args;
	char actual[500];
	plined = true;

	va_start(args, out);
	vsprintf(actual, out, args);
	aline_col(col, actual, true);
	va_end(args);
}

void aline(const char *out, bool historicise)
{
	aline_col(COL_TXT_DEF, out, historicise);
}

void line_reset()
{
	int i;

	for (i = 0; i < glnumx; ++i)
		gr_mvprintc(line_pline, 0, " ");
	msg_size_pline = 0;
}

bool pline_check()
{
	bool ret = plined;
	plined = 0;
	return ret;
}

void mlines(int num_lines, ...)
{
	va_list args;
	struct List list = LIST_INIT;

	va_start(args, num_lines);

	while (num_lines--)
		push_back(&list, va_arg(args, char *));

	va_end(args);

	mlines_list(list, num_lines);
	list_free(&list);
}

void mlines_list(struct List list, int num_lines)
{
	struct list_iter *i;
	int l_no;

	screenshot();
	if (num_lines <= 0)
		return;
	else if (num_lines == 1)
		aline(list.beg->data, true);
	else
	{
		clear_screen();
		for (l_no = 0, i = list.beg; iter_good(i); ++l_no, next_iter(&i))
		{
			mvprintw(l_no, 0, "%s", i->data);
			if (l_no == console_height - 2)
			{
				mvprintw(l_no + 1, 0, "--more--");
				getch();
				l_no = -1;
			}
		}
		move(console_height - 1, 0);
		fprintf(stdout, "--END--");
		move(console_height - 1, 7);
	}
	getch();
	move(console_height - 1, 0);
	fprintf(stdout, "       ");
}

void mask_list(struct List *ret, struct List things)
{
	struct List piles = LIST_INIT;

	/* Divide up into piles */
	item_piles(&piles, &things);

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
	free(li->prev);
}
