/* vector.c */

#include "include/vector.h"
#include <malloc.h>

Vector v_dinit ()
{
	return calloc (sizeof(struct vector_), 1);
}

Vector v_init (int mlen)
{
	Vector vec = malloc (sizeof(struct vector_));
	vec->data = malloc (sizeof(void *) * mlen);
	vec->mlen = mlen;
	vec->len = 0;
	return vec;
}

#define V_DEFAULT_LENGTH 2
#define V_NEXT_LENGTH(cur) (cur*2)
void v_push (Vector vec, void *data)
{
	if (vec->len < vec->mlen)
	{
		vec->data[vec->len] = data;
		++ vec->len;
		return;
	}
	if (vec->data == NULL)
	{
		vec->data = malloc (sizeof(void*) * V_DEFAULT_LENGTH);
		vec->mlen = V_DEFAULT_LENGTH;
		vec->len = 1;
		vec->data[0] = data;
		return;
	}
	vec->mlen = V_NEXT_LENGTH(vec->mlen);
	vec->data = realloc (vec->data, vec->mlen * sizeof(void*));
	vec->data[vec->len] = data;
	++ vec->len;
}

void v_rem (Vector vec, int rem)
{
	if (rem >= vec->len) return;
	int i;
	for (i = rem; i < vec->len; ++ i)
		vec->data[i] = vec->data[i+1];
	-- vec->len;
}

void v_free (Vector vec)
{
	free (vec->data);
	free (vec);
}

bool v_isin (Vector vec, void *data)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
		if (vec->data[i] == data) return true;
	return false;
}
