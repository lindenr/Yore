/* pline.c
 * Linden Ralph */

#include "all.h"
#include "pline.h"
#include "loop.h"
#include <stdio.h>
#include <malloc.h>

int msg_size_pline = 0, line_pline = 0;

bool plined = false;

char pask(const char *in, const char* out, ...)
{
	va_list args;
	char c;
	char o[80];
	int where = 0;
	plined = true;

	/* format the string */
	va_start(args, out);
	vsprintf(o,out,args);
	va_end(args);

	/* make the question */
	where+=strlen(o);
	memcpy(o+where, " (", 2);
	where += 2;
	memcpy(o+where, in, strlen(in));
	where += strlen(in);
	memcpy(o+where, ")", 1);
	where += 1;
	o[where] = '\0'; /* terminating null character */

	/* print the question */
	aline(o);

	/* wait for answer */
	do c = getch();
	while((!is_in(in, c)) && c != ' ' && c != 0x1B);
	return c;
}

void mvline(uint32_t yloc, uint32_t xloc, const char *txt, ...)
{
	char out[30];
	va_list args;
	int i, len;

	va_start(args, txt);
	vsprintf(out, txt, args);
	va_end  (args);
	len = strlen(out);

	if (!(yloc > 0 && yloc < 79 && xloc > 0 && xloc < 79))
	{
		/* not middle */
		return;
	}
	mvaddch(yloc-1, xloc-1, ACS_ULCORNER);
	mvaddch(yloc+1, xloc-1, ACS_LLCORNER);
	mvaddch(yloc-1, xloc,   ACS_HLINE);
	mvaddch(yloc+1, xloc,   ACS_HLINE);
	mvaddch(yloc-1, xloc+1, ACS_URCORNER);
	mvaddch(yloc+1, xloc+1, ACS_LRCORNER);
	if (xloc < 40)
	{
		/* text on RHS */
		mvaddch(yloc,   xloc-1, ACS_VLINE);
		mvaddch(yloc,   ++xloc, ACS_LTEE);
		while (xloc < 40) mvaddch(yloc, ++xloc, ACS_HLINE);
		++ xloc; mvaddch(yloc, xloc, ACS_RTEE);
		mvprintw(yloc, xloc+1, out);
		mvaddch(yloc-1, xloc, ACS_ULCORNER);
		mvaddch(yloc+1, xloc, ACS_LLCORNER);
		for (i = 0; i < len; ++ i)
		{
			mvaddch(yloc-1, xloc+i+1, ACS_HLINE);
			mvaddch(yloc+1, xloc+i+1, ACS_HLINE);
		}
		mvaddch(yloc-1, xloc+len+1, ACS_URCORNER);
		mvaddch(yloc,   xloc+len+1, ACS_VLINE);
		mvaddch(yloc+1, xloc+len+1, ACS_LRCORNER);
	}
	else
	{
		/* text on LHS */
		mvaddch(yloc,   xloc+1, ACS_VLINE);
		mvaddch(yloc,   --xloc, ACS_RTEE);
		while (xloc >= 40) mvaddch(yloc, --xloc, ACS_HLINE);
		-- xloc; mvaddch(yloc, xloc, ACS_LTEE);
		mvprintw(yloc, xloc-len, out);
		mvaddch(yloc-1, xloc, ACS_URCORNER);
		mvaddch(yloc+1, xloc, ACS_LRCORNER);
		for (i = 0; i < len; ++ i)
		{
			mvaddch(yloc-1, xloc-i-1, ACS_HLINE);
			mvaddch(yloc+1, xloc-i-1, ACS_HLINE);
		}
		mvaddch(yloc-1, xloc-len-1, ACS_ULCORNER);
		mvaddch(yloc,   xloc-len-1, ACS_VLINE);
		mvaddch(yloc+1, xloc-len-1, ACS_LLCORNER);
	}
	move(0,0);
}

void pline(const char* out, ...)
{
	va_list args;
	char *actual = malloc(sizeof(char)*500);
	plined = true;

	va_start(args, out);
	vsprintf(actual, out, args);
	aline((const char *)actual);
	va_end(args);
	free(actual);
}

void aline(const char*out)
{
	plined = true;
	if (strlen(out) > 75)
	{
		/* TODO change */
		out = "pline length exceeded";
	}
	if (msg_size_pline + strlen(out) >= 71)
	{
		mvprintw(line_pline,msg_size_pline,"--more--");
		msg_size_pline = 0;
		getch();
	}
	if (msg_size_pline == 0) CLEAR_LINE(line_pline);
	mvprintw(line_pline,msg_size_pline,"%s ", out);
	msg_size_pline += strlen(out)+1;
	refresh();
}

void line_reset()
{
	CLEAR_LINE(0);
	msg_size_pline = 0;
}

bool pline_check()
{
	bool ret = plined;
	plined = 0;
	return ret;
}
/*
char **multiline_msg, int multi_msg_size = 0;

void addlinetomsg (const char *msg)
{
	multi_msg_size ++;
	if (!multiline_msg)
	{
		multiline_msg = &msg;
		return;
	}
	char **r = malloc(sizeof(char*)*multi_msg_size);
	memcpy(r, multiline_msg, sizeof(char*)*(multi_msg_size-1));
	r[multi_msg_size-1] = msg;
	multiline_msg = r;
}

void pmsg (void)
{
	if (multi_msg_size == 0){}
	else if (multi_msg_size <= 1)
	{
		line_reset();
		pline("%s", *multiline_msg);
	}
	else if (multi_msg_size <= 7)
	{
	}
	else if (multi_msg_size < 24)
	{
	}
	else pline("Too many lines!");
	multiline_msg = NULL;
}*/

