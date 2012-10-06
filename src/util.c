/* util.c Linden Ralph */

#include "include/all.h"
#include "include/util.h"
#include "include/pline.h"
#include "include/save.h"

uint32_t Abs(int32_t i)
{
	if (i < 0)
		return ~i + 1;
	return i;
}

char *get_name(char *n)
{
	if (n[0] != '_')
		return n;
	return n + 1;
}

/* false is quit, true is stay */
bool quit()
{
	if (pask("yn", "Are you sure you want to quit -- permanently?") == 'y')
	{
		destroy_save_file(get_filename());
		return false;
	}
	return true;
}

bool is_in(const char *str, char q)
{
	unsigned size;
	unsigned i;
	size = strlen(str);
	for (i = 0; i < size; ++i)
		if (str[i] == q)
			return true;
	return false;
}
