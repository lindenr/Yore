/* words.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/words.h"

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
/*
Vector w_lines (char *msg, int line)
{
	if (line <= 0)
		return NULL;
	Vector lines = v_dinit (line+1);
	char *tmp = malloc (line+1);
	int i = 0, len = strlen (msg);
	do
	{
		memcpy (tmp, msg + line*(i++), line);
		v_pstr (lines, tmp);
	}
	while (i*line <= len);
	return lines;
}*/

