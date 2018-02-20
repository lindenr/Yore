/* pack.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/pack.h"
#include "include/item.h"
#include "include/panel.h"
#include "include/dlevel.h"
#include "include/string.h"

int PACK_AT(char a)
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
	switch (item->type.gl & 0xFF)
	{
		case ITCH_WEAPON:
			return (accepted & ITCAT_WEAPON);
		case ITCH_TOOL:
			return (accepted & ITCAT_TOOL);
		case ITCH_STRANGE:
			return (accepted & ITCAT_STRANGE);
		case ITCH_ARMOUR:
			return (accepted & ITCAT_ARMOUR);
		case ITCH_FOOD:
			return (accepted & ITCAT_FOOD);
		case ITCH_DOSH:
			return (accepted & ITCAT_DOSH);
		case ITCH_CHARM:
			return (accepted & ITCAT_CHARM);
		case ITCH_JEWEL:
			return (accepted & ITCAT_JEWEL);
	}
	panic ("item_type_flags() found a strange item type.");
	return -1;
}

int add_contents (struct String *str, Pack *pack, uint32_t accepted)
{
	if (!pack)
		return 0;
	
	int i, num_items = 0;
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++i)
	{
		struct Item *packitem = &pack->items[i];
		if (NO_ITEM(packitem))
			continue;
		if (item_type_flags (packitem, accepted))
		{
			if (!num_items)
			{
				str_catf (str, 128, "#n000BBB00%s#nBBB00000\n", item_appearance [packitem->type.gl & 0xFF]);
				//strcat (inv, item_appearance [packitem->type.gl & 0xFF]);
				//strcat (inv, "#nBBB00000\n");
			}
			char *line = get_near_desc (MTHIID(packitem->loc.inv.monsID), packitem);
			//char option[] = {'#', 'o', get_Itref (packitem), 0};//'#', 'g', 0};
			str_catf (str, 128, "#o%c%s\n", get_Itref (packitem), line);
			//strcat (inv, gl_format (packitem->type.gl));
			//strcat (inv, " ");
			//strcat (inv, line);
			//strcat (inv, "\n");
			free (line);
			++ num_items;
		}
	}
	return num_items;
}

char show_contents (Pack *pack, uint32_t accepted, char *msg)
{
	struct String *fmt = str_dinit ();
	str_catf (fmt, 256, "#nFFF00000#c%s#nBBB00000\n\n", msg);
	/*char *format = malloc(10024);
	format[0] = 0;
	strcat (format, "#nFFF00000#c");
	strcat (format, msg);
	strcat (format, "#nBBB00000\n\n");*/
	int num_items = 0;

	num_items += add_contents (fmt, pack, accepted & ITCAT_DOSH);
	num_items += add_contents (fmt, pack, accepted & ITCAT_WEAPON);
	num_items += add_contents (fmt, pack, accepted & ITCAT_ARMOUR);
	num_items += add_contents (fmt, pack, accepted & ITCAT_FOOD);
	if (!num_items)
	{
		str_free (fmt);
		p_msg ("No valid items.");
		return ' ';
	}
	char out = p_flines (fmt->data);
	str_free (fmt);
	return out;
}

void pack_get_letters (Pack pack, char *ret)
{
	unsigned k, u;
	for (u = 0, k = 0; u < MAX_ITEMS_IN_PACK; ++u)
	{
		if (!NO_ITEM(&pack.items[u]))
		{
			ret[k] = LETTER_AT(u);
			++k;
		}
	}
	strncpy (ret + k, " ?*", k+4);
}

struct Item *pack_rem (Pack *pack, unsigned u)
{
	struct Item *ret = &pack->items[u];
	ret->type.type = ITSORT_NONE;
	return ret;
}

Pack *pack_init ()
{
	Pack *pack = malloc(sizeof(Pack));
	memset (pack, 0, sizeof(Pack));
	return pack;
}

bool pack_add (Pack **ppack, struct Item *it, int u)
{
	if ((*ppack) == 0)
		*ppack = pack_init ();
	Pack *pack = *ppack;

	if (NO_ITEM(&pack->items[u]))
	{
		/* Put in pack */
		memcpy (&pack->items[u], it, sizeof(*it));
		return true;
	}
	if (!it_can_merge (MTHIID(pack->items[u].loc.inv.monsID), &pack->items[u], it))
		return false;
	pack->items[u].stacksize += it->stacksize;
	pack->items[u].cur_weight += it->cur_weight;
	return true;
}

struct Item *get_Itemc (Pack *pack, char itch)
{
	if (itch == '-')
		return &no_item;
	unsigned where = PACK_AT(itch);
	if (where == ~0u)
		return NULL;
	if (!pack)
		return NULL;
	if (NO_ITEM(&pack->items[where]))
		return NULL;
	return &pack->items[where];
}

char get_Itref (const struct Item *item)
{
	if (item->loc.loc != LOC_INV && item->loc.loc != LOC_WIELDED)
		panic ("get_Itref called on a non-inventory item");
	return LETTER_AT (item->loc.inv.invnum);
}

