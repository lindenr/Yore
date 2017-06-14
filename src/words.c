/* words.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/words.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void w_a (char *c, char *ret)
{
	char intermediate[128] = { 0, };
	if (strchr("AEIOUaeiou", c[0]))
		sprintf (intermediate, "an %s", c);
	else
		sprintf (intermediate, "a %s", c);
	strcpy (ret, intermediate); // safe if w_a safe
}

void w_the (char *end, char *c)
{
	strcpy (end, "the "); // safe if w_the safe
	strcat (end, c);
}

void w_pos (char *end, char *in)
{
	strcpy (end, in); // safe if w_pos safe
	strcat (end, "'s");
}

char *w_short (char *str, int len)
{
	int length = strlen(str);
	if (length <= len)
		return str;

	char *ret = malloc (len);
	memcpy (ret, str, len - 4);
	ret[len - 4] = '\0';
	strcat (ret, "...");
	return ret;
}

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
}

