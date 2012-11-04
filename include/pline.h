#ifndef PLINE_H_INCLUDED
#define PLINE_H_INCLUDED

#include "include/all.h"

#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "include/vector.h"

#define LINE_OF_SPACES " \
                                                                               \
"

void panel         (int);
void pline         (const char *, ...);
void pline_col     (uint32_t, const char *, ...);
void mvline        (uint32_t, uint32_t, const char *, ...);
char pask          (const char *, const char *, ...);
void line_reset    (void);
void addlinetomsg  (const char *);
void pmsg          (void);
bool pline_check   (void);
void pline_get_his (void);
void mlines        (int, ...);
void mlines_vec    (Vector);
void mask_vec      (int, Vector, Vector);

#endif /* PLINE_H_INCLUDED */
