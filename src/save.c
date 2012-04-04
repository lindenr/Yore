/* save.c
 * Linden Ralph */

#include "include/save.h"

bool save()
{
    if (pask("yn", "Save and quit?") == 'y')
    {
        pline("Saving...");
        /* TODO save the game */
        return false;
    }
    return true;
}

void destroy_save_file()
{
    /* TODO unsave the game */
}

