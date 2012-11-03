#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "include/all.h"
#include <stdbool.h>

typedef struct vector_
{
	void *data;
	int siz, len, mlen;
}
*Vector;

/* init */
Vector v_dinit (int);
Vector v_init  (int, int);

/* write */
#define v_rem(vec,i)  vector_rem  (vec, sizeof(*(vec->data)), i)
void   v_push  (Vector, void *);
void   v_rem   (Vector, int);
void   v_free  (Vector);

/* read */
bool   v_isin  (Vector, void *);

/* misc */
#define v_thing(vec,i) (((struct Thing *)(vec->data[i]))->thing)
void    v_print (Vector);

#endif /* VECTOR_H_INCLUDED */
