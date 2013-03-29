#ifndef CURSOR_H_INCLUDED
#define CURSOR_H_INCLUDED

#include "include/all.h"

enum C_Type
{
	CURS_NONE = 0,
	CURS_SMALL,
	CURS_BIG
};

struct Cursor
{
	enum C_Type type;
	int t[2];
	int status, t_change;
};

#endif /* CURSOR_H_INCLUDED */

