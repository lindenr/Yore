#ifndef WORDS_H_INCLUDED
#define WORDS_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"

int  w_a      (char *, char *, size_t); /* singular */
int  w_some   (char *, char *, int, size_t); /* lots */
void w_the    (char *, char *, size_t); /* nounified */
void w_pos    (char *, char *, size_t); /* possessive */
char *w_short (char *, int);    /* cuts with "..." if string is too long. */
Vector w_lines(const char *, int);    /* chops up a string into whitespace-separated lines */

#endif /* WORDS_H_INCLUDED */

