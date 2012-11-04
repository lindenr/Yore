/* grammar.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/grammar.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void gram_a (char *c, char *ret)
{
	char intermediate[128] = { 0, };
	if (strchr("AEIOUaeiou", c[0]))
		sprintf (intermediate, "an %s", c);
	else
		sprintf (intermediate, "a %s", c);
	strcpy (ret, intermediate);
}

void gram_the (char *end, char *c)
{
	strcpy (end, "the ");
	strcat (end, c);
}

void gram_pos (char *end, char *in)
{
	strcpy (end, in);
	strcat (end, "'s");
}

char *gram_short (char *str, int len)
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
