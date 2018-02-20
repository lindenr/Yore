#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "include/all.h"
#include <stddef.h>

typedef struct
{
	void *data;
	size_t siz, len, mlen;
} *Vector;

/* init/free */
Vector v_dinit (size_t);
Vector v_init  (size_t, size_t);
void   v_free  (Vector);

/* add */
void  *v_push  (Vector, const void *);
void  *v_pstr  (Vector, char *);
void  *v_pstrf (Vector, char *, ...);

/* remove */
void   v_rem   (Vector, size_t);
void   v_rptr  (Vector, void *);

/* misc */
#define v_at(vec,i)    (((vec)->data) + (i)*((vec)->siz))
void    v_print (Vector);

#endif /* VECTOR_H_INCLUDED */

