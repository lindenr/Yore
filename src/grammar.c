/* grammar.c
 * Linden Ralph */

#include "include/all.h"
#include "include/grammar.h"
#include "include/util.h"
#include <stdio.h>
#include <string.h>

void gram_a(char *c, char *ret)
{
    char intermediate[128] = {0,};
    if (is_in("AEIOUaeiou", c[0]))
        sprintf(intermediate, "an %s", c);
    else
        sprintf(intermediate, "a %s", c);
    strcpy(ret, intermediate);
}

void gram_the(char *end, char *c)
{
    strcpy(end, "the ");
    strcat(end, c);
}

void gram_pos(char *end, char *in)
{
    strcpy(end, in);
    strcat(end, "'s");
}

