#ifndef WORDS_H_INCLUDED
#define WORDS_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"

void w_a      (char *, char *); /* singular */
void w_pos    (char *, char *); /* possessive */
char *w_short (char *, int);    /* cuts with "..." if string is too long. */
Vector w_lines(char *, int);    /* chops up a string into whitespace-separated lines */

#endif /* WORDS_H_INCLUDED */
