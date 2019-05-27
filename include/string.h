#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include "include/all.h"

struct String
{
	char *data; /* null-terminated string of the actual data */
	size_t len; /* strlen(data); here len < mlen always */
	size_t mlen; /* amount (in bytes) of memory allocated at data */
};

/* default init */
struct String *str_dinit ();

/* init with a given alloc */
struct String *str_init (size_t mlen);

/* turn to empty string but don't deallocate memory */
void str_empty (struct String *str);

/* free all allocs */
void str_free (struct String *str);

/* safe appending a char */
void str_append (struct String *str, char ch);

/* safe concatenation */
void str_cat (struct String *str, const char *source);

/* safe formatted concatenation */
void str_catf (struct String *str, const char *fmt, ...);

/* null-terminated string containing the data */
const char *str_data (struct String *str);

#endif /* STRING_H_INCLUDED */

