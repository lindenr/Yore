#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include "include/bool.h"
#include "include/monst.h"

uint32_t Abs(int32_t);
char    *get_name(char*);
bool     quit();
bool     is_in(const char*, char);
void     screenshot(void);
void     unscreenshot(void);

#endif /* UTIL_H_INCLUDED */
