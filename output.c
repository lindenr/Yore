/* output.c
 * Linden Ralph */

#include "all.h"
#include "output.h"
#include "thing.h"
#include "mycurses.h"
#include "monst.h"
#include "loop.h"
#include <stdio.h>
#include <string.h>

uint32_t current_buffer[1920], new_buffer[1920];

void init_map  ()
{
	int i;
    for (i = 0; i < 1920; ++ i)
    {
        current_buffer[i] = '\\';
        new_buffer[i] = '/';
    }
}

void update_map()
{
	int i;
    int *is;
	char line1[80], line2[80];
	struct Thing *th = get_player();
	struct Monster *mn = th->thing;

	for (i = 0; i < 80; ++ i)
	{
		line1[i] = line2[i] = ' ';
	}
    sprintf(line1, "%s the %s                  ", mn->name+1, "<ranking>");
	sprintf(line1+26, "St:%d Dx:%d Co:%d In:%d Wi:%d Ch:%d                ", mn->attr[AB_ST], mn->attr[AB_DX],
            mn->attr[AB_CO],mn->attr[AB_IN], mn->attr[AB_WI], mn->attr[AB_CH]);
    sprintf(line1+60, "Health: %d     ", mn->HP);
    if (INT_32)
        sprintf(line2, "Time: %qu                   ", Time);
    else if (INT_64)
        sprintf(line2, "Time: %u         ", Time);

    if (U.hunger != 1) sprintf(line2+25, "%s", get_hungerstr());
	for (i = 0; i < 80; ++ i)
	{
		new_buffer[i+1680] = line1[i];
		new_buffer[i+1760] = line2[i];
	}

    is = visualise_map();

    memcpy(new_buffer, is, 1680*sizeof(uint32_t));
	free(is);
    for (i = 0; i < 1840; ++ i)
    {
        if (current_buffer[i] != new_buffer[i])
        {
            current_buffer[i] = new_buffer[i];
            mvaddch(i/80 + 1, i%80, current_buffer[i]);
        }
    }
    refresh();
}
