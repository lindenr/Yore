/* debug.c */

#include "include/debug.h"

#include <stdio.h>
#include <stdlib.h>

void panic (const char *reason)
{
	FILE *fp;

	fp = fopen ("Yore-errorlog", "w");
	fprintf (fp, "Error: %s\n", reason);
	fclose (fp);

	exit (1);
	abort ();
}
