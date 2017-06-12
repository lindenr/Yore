/* pack.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/pack.h"
#include "include/item.h"
#include "include/panel.h"

#include <malloc.h>

unsigned PACK_AT(char a)
{
	if (a < 65 || a > 122 || (a > 90 && a < 97))
		return -1;
	a -= 65;
	if (a > 26)
		a -= 32;
	return (unsigned)(a);
}

char LETTER_AT(unsigned i)
{
	if (i >= 26)
		return i + 65;
	else
		return i + 97;
}

int item_type_flags (struct Item *item, uint32_t accepted)
{
	switch (item->type.ch)
	{
		case ITEM_WEAPON:
			return (accepted & ITCAT_WEAPON);
		case ITEM_TOOL:
			return (accepted & ITCAT_TOOL);
		case ITEM_STRANGE:
			return (accepted & ITCAT_STRANGE);
		case ITEM_ARMOUR:
			return (accepted & ITCAT_ARMOUR);
		case ITEM_FOOD:
			return (accepted & ITCAT_FOOD);
		case ITEM_DOSH:
			return (accepted & ITCAT_DOSH);
		case ITEM_CHARM:
			return (accepted & ITCAT_CHARM);
		case ITEM_JEWEL:
			return (accepted & ITCAT_JEWEL);
	}
	panic ("item_type_flags() found a strange item type.");
	return -1;
}

void show_contents (struct Pack pack, uint32_t accepted)
{
	int i;
	Vector inv;

	inv = v_init (256, MAX_ITEMS_IN_PACK);
	v_pstr (inv, "Inventory");
	v_pstr (inv, "");
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++i)
	{
		if (pack.items[i] && item_type_flags (pack.items[i], accepted))
		{
			char *line = get_inv_line (pack.items[i]);
			v_pstr (inv, line);
			free (line);
		}
	}
	p_lines (inv);
	v_free (inv);
}

void pack_get_letters (struct Pack pack, char *ret)
{
	unsigned k, u;
	for (u = 0, k = 0; u < MAX_ITEMS_IN_PACK; ++u)
	{
		if (pack.items[u])
		{
			ret[k] = LETTER_AT(u);
			++k;
		}
	}
	strcpy (ret + k, " ?*");
}

struct Item *pack_rem (struct Pack *pack, char it)
{
	unsigned u = PACK_AT(it);
	struct Item *ret = pack->items[u];
	pack->items[u] = NULL;
	return ret;
}

bool pack_add (struct Pack *pack, struct Item *it)
{
	uint32_t u;
	char *msg;

	for (u = 0; u < MAX_ITEMS_IN_PACK; ++u)
	{
		if (!pack->items[u])
		{
			/* Put in pack */
			pack->items[u] = malloc (sizeof(*it));
			memcpy (pack->items[u], it, sizeof(*it));
			/* Say so */
			msg = get_inv_line (it);
			p_msg ("%s", msg);
			free (msg);
			return true;
		}
	}
	p_msg ("No space. :/");
	return false;
}

struct Item *get_Item (struct Pack pack, unsigned itnum)
{
	return pack.items[itnum];
}

struct Item *get_Itemc (struct Pack pack, char itch)
{
	unsigned where = PACK_AT(itch);
	if (where == -1)
		return NULL;
	return pack.items[where];
}

char get_Itref (struct Pack pack, struct Item *item)
{
	unsigned i;

	for (i = 0; i < MAX_ITEMS_IN_PACK; ++i)
	{
		if (pack.items[i] == item)
			return LETTER_AT(i);
	}
	return 0;
}
