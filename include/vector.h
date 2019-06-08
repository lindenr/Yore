#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "include/all.h"

struct Vector
{
	void *data;
	size_t siz, len, mlen;
};

/* init/free */
void  *v_dinit (size_t);
void  *v_init  (size_t, size_t);
void  *v_clone_ (Vector);
#define v_clone(v) v_clone_ ((void*)(v))
void   v_free_ (Vector);
#define v_free(v) v_free_((void*)(v))

/* add */
void  *v_push_ (Vector, const void *);
#define v_push(v,d) v_push_((void*)(v), (d))
void  *v_pstr  (Vector, char *);
void  *v_pstrf (Vector, char *, ...);

/* remove */
void   v_rem_  (Vector, size_t);
#define v_rem(v,s) v_rem_((void*)(v), (s))
void   v_rptr_ (Vector, void *);
#define v_rptr(v,p) v_rptr_((void*)(v), (p))

/* misc */
#define v_at(vec,i)    (((vec)->data) + (i)*((vec)->siz))
void    v_print (Vector);

#endif /* VECTOR_H_INCLUDED */

