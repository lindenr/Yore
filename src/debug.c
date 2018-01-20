/* debug.c */

#include "include/debug.h"

#include <stdio.h>
#include <stdlib.h>

void panic (const char *reason)
{
	FILE *fp;

	fp = fopen ("errorlog", "w");
	fprintf (fp, "Error: %s\n", reason);
	fclose (fp);

	fprintf (stderr, "YORE panic. See error log for details.\n");
	exit (1);
	abort ();
}

void TODO (const char *msg)
{
	fprintf (stderr, "TODO: %s\n", msg);

	exit (2);
	abort ();
}

