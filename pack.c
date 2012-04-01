/* pack.c
 * Linden Ralph */

#include "all.h"
#include "pack.h"
#include "item.h"

inline unsigned PACK_AT(char a)
{
	if (a<65 || a>122 || (a>90 && a<97)) return -1;
    a-=65;
    if (a>26) a-=32;
    return(unsigned)(a);
}

inline char LETTER_AT (unsigned i)
{
    if (i>=26) return i+65;
    else       return i+97;
}

void show_contents(struct Pack pack)
{
	int i, k;
	struct Item *it;

	screenshot();
    mvprintw(0, 40, "%-40s", "Inventory");
    mvprintw(1, 40, "%-40s", " ");
    for (i = 0, k = 1; i < 52; ++ i)
    {
        if(!pack.items[i]) continue;
		it = pack.items[i];
        mvprintw(++ k, 40, " %-40s", get_inv_line(pack.items[i]));
    }
    getch();
}

void pack_get_letters(struct Pack pack, char *ret)
{
    unsigned k, u;
    for (u = 0, k = 0; u < MAX_ITEMS_IN_PACK; ++ u)
    {
        if (pack.items[u])
        {
            ret[k] = LETTER_AT(u);
            ++ k;
        }
    }
    ret[k+0] = ' ';
    ret[k+1] = '?';
    ret[k+2] = '*';
    ret[k+3] = 0;
}

struct Item *pack_rem(struct Pack *pack, char it)
{
    unsigned u = PACK_AT(it);
    struct Item *ret = pack->items[u];
    pack->items[u] = NULL;
    return ret;
}

bool pack_add(struct Pack *pack, struct Item *it)
{
    uint32_t u;

    for (u = 0; u < MAX_ITEMS_IN_PACK; ++ u)
    {
        if (!pack->items[u])
        {
            pack->items[u] = it;
            pline("%s", get_inv_line(it));
            return true;
        }
    }
    pline("No space. :/");
    return false;
}

struct Item *get_Item(struct Pack pack, unsigned itnum)
{
	return pack.items[itnum];
}

struct Item *get_Itemc(struct Pack pack, char itch)
{
	unsigned where = PACK_AT(itch);
	if (where == -1) return NULL;
	return pack.items[where];
}

char get_Itref(struct Pack pack, struct Item *item)
{
	unsigned i;
	
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
	{
		if (pack.items[i] == item) return LETTER_AT(i);
	}
	return 0;
}
