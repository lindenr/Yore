/* words.c */

#include "include/thing.h"
#include "include/words.h"
#include "include/panel.h"
#include "include/debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int w_a (char *ret, char *c, size_t num)
{
	if (strchr("AEIOUaeiou", c[0]))
		return snprintf (ret, num, "an %s", c);
	else
		return snprintf (ret, num, "a %s", c);
}

int w_some (char *ret, char *c, int stacksize, size_t num)
{
	if (stacksize <= 0)
	{
		panic("stack size <= 0 in w_some");
		return 0;
	}

	if (stacksize == 1)
		return w_a (ret, c, num);

	return snprintf (ret, num, "%d %s", stacksize, c);
}

void w_the (char *end, char *c, size_t num)
{
	strncpy (end, "the ", num);
	strncat (end, c, num-4);
}

void w_pos (char *end, char *in, size_t num)
{
	strncpy (end, in, num-1);
	end[num-1] = 0;
	strncat (end, "'s", num-strlen(end));
}

char *w_short (char *str, int len)
{
	int length = strlen(str);
	if (length <= len)
		return str;

	char *ret = malloc (len);
	memcpy (ret, str, len - 4);
	ret[len - 4] = '\0';
	strncat (ret, "...", 4);
	return ret;
}

Vector w_lines (const char *msg, int max)
{
	if (max <= 0)
		return NULL;
	Vector lines = v_dinit (max+1);
	char *tmp = malloc (max+1);
	//tmp[line] = 0;

	int i, start = 0, end = -1, len = 0;
	for (i = 0; msg[i]; ++ i, ++ len)
	{
		if (msg[i] == ' ' || msg[i] == '\n')
			end = i;
		if (len >= max || msg[i] == '\n')
		{
			if (end <= start && msg[i] != '\n')
			{
				printf("sadfasdfasdfasdfasdf\n");
				p_msg ("line too short");
				return lines;
			}
			memcpy (tmp, msg + start, end-start);
			tmp[end-start] = 0;
			v_pstr (lines, tmp);
			len -= end + 1 - start;
			start = end + 1;
		}
	}
	strncpy (tmp, msg + start, len+1);
	v_pstr (lines, tmp);
	free (tmp);
	return lines;
/*
	int i = 0, len = strlen (msg);
	do
	{
		memcpy (tmp, msg + line*i, line);
		++ i;
		v_pstr (lines, tmp);
	}
	while (i*line <= len);
	return lines;*/
}

