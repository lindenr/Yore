/* panel.c */

#include "include/panel.h"
#include "include/graphics.h"

#include <stdlib.h>
#include <malloc.h>

int p_width = 25, p_height = 12, p_open = 0;

char *p_tabs[NUM_TABS] = {0, };

void p_init ()
{
	int i;
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

	if (state < 0 || state > NUM_TABS)
		return;

	for (j = glnumx - p_width; j < glnumx; ++ j)
		txt_mvaddch (p_height, j, ACS_HLINE);
	for (i = p_height+1; i < glnumy; ++ i)
	{
		for (j = glnumx - p_width; j < glnumx; ++ j)
			txt_mvaddch (i, j, p_tabs[state][(i - p_height)*p_width + j - glnumx]);
	}
}

void p_msg (char *msg, ...)
{
}

char p_ask (char *results, char *question)
{
	return results[0];
}

void p_lines (Vector lines)
{
}

