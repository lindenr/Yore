#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdbool.h>
#include "include/monst.h"

#define YORE_VERSION "0.0.5 pre-alpha"

uint32_t Abs(int32_t);
char *get_name(char *);
bool is_in(const char *, char);
void screenshot(void);
void unscreenshot(void);

#endif /* UTIL_H_INCLUDED */
