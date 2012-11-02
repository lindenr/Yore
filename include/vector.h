#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "include/all.h"
#include "include/bool.h"

struct vector_
{
	void **data;
	int len;
	int mlen;
};

typedef struct vector_ *Vector;

/* init */
Vector v_dinit ();
Vector v_init  (int);

/* write */
void   v_push  (Vector, void *);
void   v_rem   (Vector, int);
void   v_free  (Vector);

/* read */
bool   v_isin  (Vector, void *);

/* misc */
#define v_thing(vec,i) (((struct Thing *)(vec->data[i]))->thing)

#endif /* VECTOR_H_INCLUDED */
