#if !defined(SAVE_C_INCLUDED)
#define SAVE_C_INCLUDED

#include <stdbool.h>

bool save(char *);
bool quit();
void destroy_save_file(char *);
void restore(char *);

#endif /* SAVE_C_INCLUDED */
