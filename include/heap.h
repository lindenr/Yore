#ifndef HEAP_H_INCLUDED
#define HEAP_H_INCLUDED

#include "include/all.h"
#include <stddef.h>

/* predicate type; should be strict and total but not necessarily trichotomous */
typedef int (*h_pred) (const void *, const void *);

struct Heap
{
	void *data; /* array of data, containing: */
	size_t siz; /* elements of this size */
	size_t len; /* this many of them */
	size_t mlen; /* with space for this many in total */
	h_pred cmp; /* ordered by this predicate */
};

/* default init */
struct Heap *h_dinit_aux (size_t siz, h_pred pred);

/* explicit casting for default init */
#define h_dinit(siz,pred) (h_dinit_aux((siz), (h_pred)(pred)))

/* init with a given alloc */
struct Heap *h_init_aux (size_t siz, h_pred pred, size_t mlen);

/* explicit casting for init */
#define h_init(siz,pred,mlen) (h_init_aux((siz), (h_pred)(pred), (mlen)))

/* free all allocs */
void h_free (struct Heap *heap);

/* insert an element */
void *h_push (struct Heap *heap, const void *data);

/* get reference to bottom element */
const void *h_least (const struct Heap *heap);

/* remove bottom element and write it to out */
void h_pop (struct Heap *heap, void *out);

/* empty heap */
void h_empty (struct Heap *heap);

/* random access (not for normal use) */
void *h_at (struct Heap *heap, size_t at);

#endif /* HEAP_H_INCLUDED */

