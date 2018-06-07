/* heap.c */

#include "include/heap.h"
#include <malloc.h>
#include <string.h>

#define H_DEFAULT_SIZ 3
struct Heap *h_dinit_aux (size_t siz, h_pred pred)
{
	return h_init_aux (siz, pred, H_DEFAULT_SIZ);
}

struct Heap *h_init_aux (size_t siz, h_pred pred, size_t mlen)
{
	struct Heap *h = malloc (sizeof (struct Heap));
	*h = (struct Heap) {malloc (mlen * siz), siz, 0, mlen, pred};
	return h;
}

void h_free (struct Heap *h)
{
	free (h->data);
	free (h);
}

#define H_NEXT_LENGTH(cur) (1 + (cur)*2)
#define H_PAR(i) (((i)-1)/2)
void *h_push (struct Heap *h, const void *data)
{
	if (h->len >= h->mlen)
	{
		h->mlen = H_NEXT_LENGTH (h->mlen);
		h->data = realloc (h->data, h->mlen * h->siz);
	}
	++ h->len;
	int i;
	for (i = h->len-1; h->cmp (data, h_at (h, H_PAR(i))) && i; i = H_PAR(i))
		memcpy (h_at (h, i), h_at (h, H_PAR(i)), h->siz);
	memcpy (h_at (h, i), data, h->siz);
	return h_at (h, i);
}

const void *h_least (const struct Heap *h)
{
	return h->data;
}

void h_pop (struct Heap *h, void *out)
{
	if (!h->len)
		return;
	if (out)
		memcpy (out, h->data, h->siz);
	int i;
	void *last = h->data + (h->len - 1) * h->siz;
	for (i = 0; i < h->len; )
	{
		int j1 = 2*i + 1, j2 = 2*i + 2, j;
		int a1 = j1 < h->len - 1 && h->cmp (h_at (h, j1), last);
			j = j1;
		int a2 = j2 < h->len - 1 && h->cmp (h_at (h, j2), last);
			j = j2;
		if (a1 == 0 && a2 == 0)
			break;
		else if (a1 != 0 && a2 == 0)
			j = j1;
		else if (a1 == 0 && a2 != 0)
			j = j2;
		else
		{
			j = j1;
			if (h->cmp (h_at (h, j2), h_at (h, j1)))
				j = j2;
		}
		memcpy (h_at (h, i), h_at (h, j), h->siz);
		i = j;
	}
	memcpy (h_at (h, i), last, h->siz);
	h->len --;
}

void h_empty (struct Heap *h)
{
	h->len = 0;
}

void *h_at (struct Heap *h, size_t i)
{
	return h->data + i * h->siz;
}

