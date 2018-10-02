#if !defined(DEBUG_H_INCLUDED)
#define DEBUG_H_INCLUDED

void panic (const char *msg); /* abort; writes to error file */
void TODO  (const char *msg); /* abort (use for code stubs); writes to stderr */

#define DUMP(str) fprintf (stderr, "%s\n", str)

#endif /* DEBUG_H_INCLUDED */

