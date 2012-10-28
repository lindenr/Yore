/* save.c */

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
#include "include/graphics.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

FILE *game_save_file;

/* Saves a native type (eg int, char). Not pointers. Also, multiple evaluations. */
#define SAVE_NATIVE(n) fwrite(&(n), sizeof(n), 1, game_save_file)
/*
struct ref_it_type
{
	char type;
	uint32_t dat;
};

struct List blocks = LIST_INIT;

void store_type(struct item_struct *ptr)
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
}*/

void save_item(struct Item *item)
{
	void *p = NULL;
	//if (!item)
	{
		fwrite(&p, sizeof(p), 1, game_save_file);
		return;
	}
	//store_type(item->type);
	SAVE_NATIVE(item->type);
	SAVE_NATIVE(item->attr);
	SAVE_NATIVE(item->cur_weight);

	if (!item->name)
		fwrite("\0", 1, 1, game_save_file);
	else
		fwrite(item->name, strlen(item->name)+1, 1, game_save_file);
}

/* The three main things needing saving are: the List all_things, the uint64_t Time, and
 * the player_struct U. There is also the list of things you have seen.
 * This function saves in the following order: Time; U; all_things; sq_attr. */
bool save(char *filename)
{
	int i;
	if (pask("yn", "Save and quit?") == 'y')
	{
		pline("Saving...");
		game_save_file = fopen(filename, "wb");
		fwrite("YOREv"YORE_VERSION, sizeof("YOREv"YORE_VERSION), 1, game_save_file);

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
		SAVE_NATIVE(U.magic);
		
		/* all_things */
		ITER_THINGS(li, num)
		{
			struct Thing *th = li->data;
			int enum_saver = (int)(th->type);
			assert(enum_saver);
			SAVE_NATIVE(enum_saver);
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
						fwrite(mn->name, 20, 1, game_save_file);
					else
						fwrite("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 20, 1, game_save_file);

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
					save_item(mn->wearing.rweap);
					save_item(mn->wearing.lweap);
					SAVE_NATIVE(mn->wearing.two_weaponing);

					SAVE_NATIVE(mn->status);
					save_item(mn->eating);
					break;
				}
				case THING_DGN:
				{
					struct map_item_struct *mapit = th->thing;
					long type = GETMAPITEMID(mapit->ch);
					SAVE_NATIVE(type);
				}
				default:
				{
				}
			}
		}
		fprintf(game_save_file, "1111");

		for (i = 0; i < 1680; ++ i) fprintf(game_save_file, "%c", sq_seen[i]);
		
		//save_block();
		
		fclose(game_save_file);
		return false;
	}
	return true;
}

#define LOAD_NATIVE(n) fread(&(n), sizeof(n), 1, game_save_file)
/*
struct List load_blocks = LIST_INIT;

void *store_load_block(void **mem, int len)
{
	struct block_ptr *bp = malloc(sizeof(*bp));
	bp->ptr = mem;
	bp->len = len;
	push_back(&blocks, bp);
	return *mem;
}

void load_block()
{
}*/

void load_item(struct Item **out_item)
{
	void *ptr;
	LOAD_NATIVE(ptr);
	if (!ptr)
	{
		*out_item = NULL;
		return;
	}
	struct Item *item = malloc(sizeof(*item));
	item->type = items;
	//store_load_block(&item->type, ptr, sizeof(*item->type));
	LOAD_NATIVE(item->attr);
	LOAD_NATIVE(item->cur_weight);
	char temp[100];
	fgets(temp, 100, game_save_file);
	int len = strlen(temp);
	if (len < 2) item->name = NULL;
	else
	{
		item->name = malloc(len+1);
		strcpy(item->name, temp);
	}

	*out_item = item;
}

void restore(char *filename)
{
	int i;
	U.playing = PLAYER_ERROR; /* for premature returning */
	game_save_file = fopen(filename, "rb");
	if (!game_save_file) panic("Save file incorrect\n");
	
	char *ftest = malloc(strlen("YOREv"YORE_VERSION))+1;

	fread(ftest, strlen("YOREv"YORE_VERSION)+1, 1, game_save_file);
	if (strcmp(ftest, "YOREv"YORE_VERSION)) panic("Save file incorrect\n");

	LOAD_NATIVE(Time);

	LOAD_NATIVE(U.hunger);
	U.player = NULL;
	LOAD_NATIVE(U.role);
	LOAD_NATIVE(U.playing);
	for (i = 0; i < 6; ++ i)
		LOAD_NATIVE(U.attr[i]);
	LOAD_NATIVE(U.luck);
	LOAD_NATIVE(U.m_glflags);
	LOAD_NATIVE(U.magic);

	while (1)
	{
		struct Thing *th = malloc(sizeof(*th));
		if (!U.player) U.player = th;
		LOAD_NATIVE(th->type);
		if (th->type == 0x31313131)
		{
			free(th);
			break;
		}
		if (th->type > 6) panic("Loading files is messed up.");
		LOAD_NATIVE(th->yloc);
		LOAD_NATIVE(th->xloc);
		
		switch(th->type)
		{
			case THING_ITEM:
			{
				struct Item *it = th->thing;
				load_item(&it);
				break;
			}
			case THING_MONS:
			{
				struct Monster *mn = malloc(sizeof(*mn));

				LOAD_NATIVE(mn->type);
				LOAD_NATIVE(mn->level);
				LOAD_NATIVE(mn->exp);
				LOAD_NATIVE(mn->HP);
				LOAD_NATIVE(mn->HP_max);
				LOAD_NATIVE(mn->cur_speed);
				mn->name = malloc(20);
				fread(mn->name, 20, 1, game_save_file);

				for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
					load_item(&(mn->pack.items[i]));

				load_item(&mn->wearing.head);
				load_item(&mn->wearing.torso);
				load_item(&mn->wearing.legs);
				load_item(&mn->wearing.feet);
				load_item(&mn->wearing.hands);
				load_item(&mn->wearing.arms);
				load_item(&mn->wearing.rfin);
				load_item(&mn->wearing.lfin);
				load_item(&mn->wearing.rweap);
				load_item(&mn->wearing.lweap);
				LOAD_NATIVE(mn->wearing.two_weaponing);

				LOAD_NATIVE(mn->status);
				load_item(&mn->eating);

				th->thing = mn;
				break;
			}
			case THING_DGN:
			{
				long num;
				struct map_item_struct *mapit = malloc(sizeof(*mapit));
				LOAD_NATIVE(num);
				memcpy(mapit, &map_items[num], sizeof(*mapit));
				th->thing = mapit;
				break;
			}
			default:
			{
			}
		}
		push_back(&all_things[to_buffer(th->yloc, th->xloc)], th);
	}

	for (i = 0; i < 1680; ++ i) LOAD_NATIVE(sq_seen[i]);

	//load_block();
	
	fclose (game_save_file);

	fclose (game_save_file);

	U.playing = PLAYER_PLAYING;	/* success */
}

/* false is quit, true is stay */
bool quit ()
{
	if (pask("yn", "Are you sure you want to quit -- permanently?") == 'y')
	{
		printf ("Quitting...\n");
		destroy_save_file (get_filename());
		return false;
	}
	return true;
}

#if defined(__WIN32__)
#  define SH_RM "del"
#else
#  define SH_RM "rm"
#endif

void destroy_save_file (char *filename)
{
	char str[1000];

    FILE *file = fopen (filename, "wb+");
    fwrite ("", 1, 1, file);
    fclose (file);

	sprintf (str, "%s %s", SH_RM, filename);
	system (str);
}
