#ifndef PLINE_H_INCLUDED
#define PLINE_H_INCLUDED

#include <stdarg.h>
#include <string.h>

#include "bool.h"

#define LINE_OF_SPACES " \
                                                                               \
"

void     aline  (const char*);
void     pline  (const char*, ...);
void     mvline (uint32_t, uint32_t, const char*, ...);
char     pask   (const char*, const char*, ...);
void     line_reset(void);
void     addlinetomsg(const char *);
void     pmsg   (void);

#endif /* PLINE_H_INCLUDED */
