/* save.c Linden Ralph */

#include "include/all.h"
#include "include/save.h"
#include "include/pline.h"
#include "include/monst.h"

#include <stdlib.h>

bool save(char *filename)
{
	if (pask("yn", "Save and quit?") == 'y')
	{
		pline("Saving...");
		// TODO save the game
		return false;
	}
	return true;
}

void restore(char *filename)
{
	// TODO restore the game
	U.playing = PLAYER_PLAYING;	/* success */
}

void destroy_save_file(char *filename)
{
#if defined(WINDOWS)
	DeleteFile(filename);
#elif defined(FOONIX)
	char str[1000];
	sprintf(str, "rm %s", filename);
	system(str);
#endif
}
