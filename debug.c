/* debug.c
 * Linden Ralph */

#include "debug.h"

#if defined(DEBUGGING)

#include <stdio.h>

FILE *fp;

void debug_init(const char *filename)
{
	fp = fopen(filename, "w");
}

void debug_log_error(enum DEBUG_ERROR_REASON der, const char *reason)
{
	fprintf(fp, "Error %d: %s\n", der, reason);
}

void debug_end()
{
	fclose(fp);
}

#endif /* DEBUGGING */