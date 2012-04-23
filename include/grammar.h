#if !defined(GRAMMAR_H_INCLUDED)
#define GRAMMAR_H_INCLUDED

#include "all.h"

void  gram_a   (char *, char *); /* singular */
void  gram_pos (char *, char *); /* possessive */
char *gram_short(char *, int);   /* cuts with "..." if string is too long. */

#endif /* GRAMMAR_H_INCLUDED */
