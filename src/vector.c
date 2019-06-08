/* vector.c */

#include "include/vector.h"
#include "include/debug.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <malloc.h>

#define V_DEFAULT_LENGTH 2
void *v_dinit (size_t siz)
{
	return v_init (siz, V_DEFAULT_LENGTH);
}

void *v_init (size_t siz, size_t mlen)
{
	Vector vec = malloc (sizeof(*vec));
	vec->data = malloc (siz * mlen);
	vec->siz = siz;
	vec->len = 0;
	vec->mlen = mlen;
	return vec;
}

void *v_clone_ (Vector from)
{
	Vector vec = malloc (sizeof(*vec));
	*vec = (struct Vector) {
		malloc (from->siz * from->mlen),
		from->siz,
		from->len,
		from->mlen
	};
	memcpy (vec->data, from->data, from->siz*from->len);
	return vec;
}

#define V_NEXT_LENGTH(cur) (1 + (cur)*2)
#define DATA(i)            (vec->data + ((i)*(vec->siz)))
void *v_push_ (Vector vec, const void *data)
{
	if (vec->data == NULL)
		panic("NULL vector");
	if (vec->len >= vec->mlen)
	{
		vec->mlen = V_NEXT_LENGTH(vec->mlen);
		vec->data = realloc (vec->data, vec->mlen * vec->siz);
	}
	memcpy (DATA(vec->len), data, vec->siz);
	++ vec->len;
	return v_at (vec, vec->len - 1);
}

void *v_pstr (Vector vec, char *data)
{
	if (vec->data == NULL)
		panic("NULL str vector");
	if (vec->len >= vec->mlen)
	{
		vec->mlen = V_NEXT_LENGTH(vec->mlen);
		vec->data = realloc (vec->data, vec->mlen * vec->siz);
	}

	strncpy (DATA(vec->len), data, vec->siz);
	((char*) DATA(vec->len))[vec->siz-1] = 0;

	++ vec->len;
	return v_at (vec, vec->len - 1);
}

void *v_pstrf (Vector vec, char *data, ...)
{
	if (vec->data == NULL)
		panic("NULL str vector");
	if (vec->len >= vec->mlen)
	{
		vec->mlen = V_NEXT_LENGTH(vec->mlen);
		vec->data = realloc (vec->data, vec->mlen * vec->siz);
	}

	va_list args;
	va_start (args, data);
	vsnprintf (DATA(vec->len), vec->siz, data, args);
	va_end (args);

	++ vec->len;
	return v_at (vec, vec->len - 1);
}

void v_rem_ (Vector vec, size_t rem)
{
	int i;
	if (rem >= vec->len) return;

	for (i = rem; i < vec->len - 1; ++ i)
		memcpy (DATA(i), DATA(i+1), vec->siz);
	-- vec->len;
}

void v_rptr_ (Vector vec, void *data)
{
	uintptr_t p = (uintptr_t) data;
	if (p < (uintptr_t) vec->data || p >= (uintptr_t) vec->data + vec->len * vec->siz)
		return;
	p -= (uintptr_t) vec->data;
	p /= vec->siz;
	v_rem (vec, p);
}

void v_free_ (Vector vec)
{
	free (vec->data);
	free (vec);
}

void v_print (Vector vec)
{
	int i;
	printf("%p %zu %zu %zu\n", vec->data, vec->siz, vec->len, vec->mlen);
	for (i = 0; i < vec->len; ++ i) printf("%d, ", *(int*)DATA(i));
	printf("\n");
}

