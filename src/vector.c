/* vector.c */

#include "include/vector.h"

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#define V_DEFAULT_LENGTH 2
Vector v_dinit (int siz)
{
	return v_init (siz, V_DEFAULT_LENGTH);
}

Vector v_init (int siz, int mlen)
{
	Vector vec = malloc (sizeof(*vec));
	vec->data = malloc (siz * mlen);
	vec->siz = siz;
	vec->len = 0;
	vec->mlen = mlen;
	return vec;
}

#define V_NEXT_LENGTH(cur) (cur*2)
#define DATA(i)            (vec->data + ((i)*(vec->siz)))
void *v_push (Vector vec, void *data)
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

void v_rem (Vector vec, int rem)
{
	int i;
	if (rem >= vec->len) return;

	for (i = rem; i < vec->len; ++ i)
		memcpy (DATA(i), DATA(i+1), vec->siz);
	-- vec->len;
}

void v_free (Vector vec)
{
	free (vec->data);
	free (vec);
}

void v_print (Vector vec)
{
	int i;
	printf("%p %d %d %d\n", vec->data, vec->siz, vec->len, vec->mlen);
	for (i = 0; i < vec->len; ++ i) printf("%d, ", *(int*)DATA(i));
	printf("\n");
}

bool v_isin (Vector vec, void *data)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
		if (memcmp (DATA(i), data, vec->siz) == 0) return true;
	return false;
}
