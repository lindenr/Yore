/* string.c */

#include "include/string.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

#define S_DEFAULT_LENGTH 128
struct String *str_dinit ()
{
	return str_init (S_DEFAULT_LENGTH);
}

struct String *str_init (size_t mlen)
{
	if (mlen == 0)
		mlen = 1;
	struct String *str = malloc (sizeof(struct String));
	str->data = malloc (mlen);
	str->data[0] = 0;
	str->len = 0;
	str->mlen = mlen;
	return str;
}

void str_free (struct String *str)
{
	free (str->data);
	free (str);
}

#define S_NEXT_LENGTH(n) ((n)*2)
void str_append (struct String *str, char ch)
{
	if (str->len >= str->mlen - 1)
	{
		size_t mlen = S_NEXT_LENGTH(str->mlen);
		str->data = realloc (str->data, mlen);
		str->mlen = mlen;
	}
	str->data[str->len] = ch;
	str->data[str->len+1] = 0;
	str->len ++;
}

void str_cat (struct String *str, size_t max, const char *source)
{
	int i;

	/*if (str->len + max < str->mlen)
	{
		strncat (str->data + str->len, source, max);
		str->len += ?? TODO
		return;
	}*/

	for (i = 0; i < max && source[i]; ++ i)
		str_append (str, source[i]);
}

void str_catf (struct String *str, size_t max, const char *fmt, ...)
{
	char *mystr = malloc (max);

	va_list args;
	va_start (args, fmt);
	vsnprintf (mystr, max+1, fmt, args);
	va_end (args);

	str_cat (str, max, mystr);
	free (mystr);
}


