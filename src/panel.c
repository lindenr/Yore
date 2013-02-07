/* panel.c */

#include "include/panel.h"
#include "include/thing.h"
#include "include/graphics.h"

#include <stdlib.h>
#include <malloc.h>

int p_width = 50, p_height = 12;
int p_open = 0;

char *p_tabs[NUM_TABS] = {NULL, };
Vector messages = NULL;

void p_pane ()
{
	int i, j;
	for (i = 0; i < p_height; ++ i)
		for (j = glnumx - p_width; j < glnumx; ++ j)
			txt_mvaddch (i, j, ' ');

	txt_box (0, glnumx - p_width, p_height-1, p_width-1);
	txt_mvprint (1, glnumx - p_width + 1, "HP %d:%d", pmons.HP, pmons.HP_max);
}

void p_init ()
{
//	txt_mvprint (0, 0, "ASDFASDFASDF");
//	gr_refresh ();
//	return;
	int i;
	if (!messages)
		messages = v_dinit (1024);

	for (i = 0; i < NUM_TABS; ++ i)
	{
		p_tabs[i] = realloc (p_tabs[i], p_width * (glnumy - p_height));
		memset (p_tabs[i], ' ', p_width * (glnumy - p_height));
	}
}

void p_tab (int state)
{
	int i, j;
	p_open = state;

	if (state == 0)
	{
		for (j = glnumx - p_width; j < glnumx; ++ j)
			txt_mvaddch (p_height, j, ' ');
		for (i = p_height+1; i < glnumy; ++ i)
		{
			for (j = glnumx - p_width; j < glnumx; ++ j)
				txt_mvaddch (i, j, 0); /* clear it */
		}
		return;
	}

	p_update ();
}

void p_update ()
{
	int i, j;
	if (p_open <= 0 || p_open > NUM_TABS)
		return;

	for (j = glnumx - p_width; j < glnumx; ++ j)
		txt_mvaddch (p_height, j, ACS_HLINE);
	for (i = p_height+1; i < glnumy; ++ i)
	{
		for (j = glnumx - p_width; j < glnumx; ++ j)
			txt_mvaddch (i, j, p_tabs[p_open][(i - p_height)*p_width + j - glnumx]);
	}
}

void p_messages_display ()
{
	if (!messages)
		return;

	int i;
	for (i = 0; i < messages->len; ++ i)
	{
	}
}

void p_msg (char *str, ...)
{
	va_list args;
	char out[1024];

	va_start (args, str);
	vsprintf (out, str, args);
	va_end (args);

	v_pstr (messages, out);
	p_update ();
}

char p_ask (char *results, char *question)
{
	return results[0];
}

void p_lines (Vector lines)
{
}

