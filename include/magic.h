#ifndef MAGIC_H_INCLUDED
#define MAGIC_H_INCLUDED

#include "include/all.h"
#include "include/monst.h"
#include "include/bool.h"

void magic_spell (struct Monster *, char);
bool magic_isspell (char);

#endif /* MAGIC_H_INCLUDED */