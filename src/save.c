/* save.c Linden Ralph */

#include "include/all.h"
#include "include/save.h"
#include "include/pline.h"
#include "include/monst.h"
#include "include/loop.h"
#include "include/list.h"
#include "include/thing.h"
#include "include/pack.h"
#include "include/util.h"
#include "include/map.h"

#include <stdlib.h>
#include <stdio.h>

const uint16_t ENDIAN_TEST = 1;
#define WRONG_ENDIAN (*((char*)(&ENDIAN_TEST)) == 0)

FILE *game_save_file;

/* convert to corrent endianness, should be little-endian */
void memconv(void *m_, int size)
{
	if (!WRONG_ENDIAN) return;
	char *mem = m_;
	int i; -- size;
	char tmp;
	for (i = 0; i < size; ++ i, -- size)
	{
		tmp = *(mem+i);
		*(mem+i) = *(mem+size);
		*(mem+size) = tmp;
	}
}

/* Saves a native type (eg int, char). Not pointers. Also, multiple evaluations.*/
#define SAVE_NATIVE(n) memconv(&n, sizeof(n)); fwrite(&n, sizeof(n), 1, game_save_file)

struct block_ptr
{
	void *ptr;
	int len;
};

struct List blocks;

void store_block(void *ptr, int len)
{
	struct block_ptr *bp = malloc(sizeof(*bp));
	bp->ptr = ptr;
	bp->len = len;
	push_back(&blocks, bp);
}

void save_block()
{
	struct list_iter *li;
	fwrite("1111", 4, 1, game_save_file);
	for (li = blocks.beg; iter_good(li); next_iter(&li))
	{
		struct block_ptr *bp = li->data;
		SAVE_NATIVE(bp->ptr);
		fwrite(bp->ptr, bp->len, 1, game_save_file);
	}
}

void save_item(struct Item *item)
{
	if (!item)
	{
		fwrite("\0\0\0\0", 1, 1, game_save_file);
		return;
	}
	store_block(item->type, sizeof(struct item_struct));
	SAVE_NATIVE(item->type);
	SAVE_NATIVE(item->attr);
	SAVE_NATIVE(item->cur_weight);
	fwrite(item->name, strlen(item->name)+1, 1, game_save_file);
}

/* This directly converts currently-used memory to little-endian; on some machines it *will*
 * render the game unplayable. Hence, if the user saves their game, it will quit as soon as possible. */
/* The three main things needing saving are: the List all_things, the uint64_t Time, and
 * the player_struct U. This function saves in the following order: Time; U; all_things. */
bool save(char *filename)
{
	int i;
	struct list_iter *li;
	if (pask("yn", "Save and quit?") == 'y')
	{
		pline("Saving...");
		game_save_file = fopen("YORE_SAVE.dat", "w");
		fwrite("YOREv"YORE_VERSION, sizeof("YOREv"YORE_VERSION)+1, 1, game_save_file);

		/* Time */
		SAVE_NATIVE(Time);

		/* U */
		SAVE_NATIVE(U.hunger);
		SAVE_NATIVE(U.role);
		SAVE_NATIVE(U.playing);
		for (i = 0; i < 6; ++ i)
			SAVE_NATIVE(U.attr[i]);
		SAVE_NATIVE(U.luck);
		SAVE_NATIVE(U.m_glflags);
		
		/* all_things */
		for (li = all_things.beg; iter_good(li); next_iter(&li))
		{
			struct Thing *th = li->data;
			SAVE_NATIVE(th->type);
			SAVE_NATIVE(th->yloc);
			SAVE_NATIVE(th->xloc);
			switch (th->type)
			{
				case THING_ITEM:
				{
					save_item(th->thing);
					break;
				}
				case THING_MONS:
				{
					struct Monster *mn = th->thing;
					SAVE_NATIVE(mn->type);
					SAVE_NATIVE(mn->level);
					SAVE_NATIVE(mn->exp);
					SAVE_NATIVE(mn->HP);
					SAVE_NATIVE(mn->HP_max);
					SAVE_NATIVE(mn->cur_speed);

					if (mn->name)
						fwrite(mn->name, strlen(mn->name)+1, 1, game_save_file);
					else
						fwrite("\0", 1, 1, game_save_file);

					/* pack */
					for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
						save_item(mn->pack.items[i]);

					/* wearing */
					save_item(mn->wearing.head);
					save_item(mn->wearing.torso);
					save_item(mn->wearing.legs);
					save_item(mn->wearing.feet);
					save_item(mn->wearing.hands);
					save_item(mn->wearing.arms);
					save_item(mn->wearing.rfin);
					save_item(mn->wearing.lfin);
					save_item(mn->wearing.head);
					save_item(mn->wearing.head);
					SAVE_NATIVE(mn->wearing.two_weaponing);

					SAVE_NATIVE(mn->status);
					save_item(mn->eating);
					break;
				}
				case THING_DGN:
				{
					/* Where is it in the array? */
					int type = (int)th->thing;
					type -= (int)map_items;
					type /= sizeof(struct map_item_struct);
					SAVE_NATIVE(type);
				}
				default:
				{
				}
			}
		}
		
		save_block();
		
		fclose(game_save_file);
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
