/* debug.c
 * Linden Ralph */

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

FILE *fp;

void debug_init(const char *filename)
{
	fp = fopen(filename, "w");
}

void panic(const char *reason)
{
	fprintf(fp, "Error: %s\n", reason);
    fclose(fp);
    exit(1);
    abort();
}

void debug_end()
{
	fclose(fp);
}

