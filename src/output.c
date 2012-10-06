/* output.c Linden Ralph */

#include "include/all.h"
#include "include/output.h"
#include "include/thing.h"
#include "include/mycurses.h"
#include "include/monst.h"
#include "include/loop.h"
#include "include/rank.h"
#include "include/grammar.h"

#include <stdio.h>
#include <string.h>

uint32_t current_buffer[1920], new_buffer[1920];

void init_map()
{
	int i;
	for (i = 0; i < 1920; ++i)
	{
		current_buffer[i] = '\\';
		new_buffer[i] = '/';
	}
}

void update_map()
{
	int i;
	char line1[80], line2[80];
	struct Thing *th = get_player();
	struct Monster *mn = th->thing;

	for (i = 0; i < 80; ++i)
	{
		line1[i] = line2[i] = ' ';
	}

	/* Random '\0's will be sprinkled throughout line1 and line2 */
	sprintf(line1, "%s the %s", gram_short(mn->name + 1, 15), get_rank());
	sprintf(line1 + 26, "St:%d Dx:%d Co:%d In:%d Wi:%d Ch:%d", U.attr[AB_ST],
			U.attr[AB_DX], U.attr[AB_CO], U.attr[AB_IN], U.attr[AB_WI],
			U.attr[AB_CH]);
	sprintf(line1 + 65, "Health: %d:%d", mn->HP, mn->HP_max);

#   if INT_SIZE == 32
	sprintf(line2, "Time: %qu", Time);
#   elif INT_SIZE == 64
	sprintf(line2, "Time: %u", Time);
#   endif

	sprintf(line2 + 26, "Level %d:%d", mn->level, mn->exp);

	sprintf(line2 + 65, "%s", get_hungerstr());

	for (i = 0; i < 80; ++i)
	{
		if (line1[i] == '\0')
			line1[i] = ' ';
		new_buffer[i + 1680] = line1[i];
		if (line2[i] == '\0')
			line2[i] = ' ';
		new_buffer[i + 1760] = line2[i];
	}

	/* sets new_buffer directly */
	visualise_map();

	for (i = 0; i < 1840; ++i)
	{
		if (current_buffer[i] != new_buffer[i])
		{
			current_buffer[i] = new_buffer[i];
			mvaddch(i / 80 + 1, i % 80, current_buffer[i]);
		}
	}
	refresh();
}
