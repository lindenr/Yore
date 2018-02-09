/* pack.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/pack.h"
#include "include/item.h"
#include "include/panel.h"
#include "include/dlevel.h"

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

char show_contents (Pack *pack, uint32_t accepted, char *msg)
{
	int i, num_items = 0;
	Vector inv;
	char first[40] = "                                       ";

	inv = v_init (sizeof(struct MenuOption), MAX_ITEMS_IN_PACK);

	int msglen = strlen(msg);
	snprintf (first + (40-msglen)/2, 40, "%s", msg);
	struct MenuOption m = (struct MenuOption) {0, {0,}, NULL};
	gr_ext (m.ex_str, first);
	v_push (inv, &m);
	m = (struct MenuOption) {0, {0,}, NULL};
	v_push (inv, &m);
	if (pack)
	{
		for (i = 0; i < MAX_ITEMS_IN_PACK; ++i)
		{
			struct Item *packitem = &pack->items[i];
			if (NO_ITEM(packitem))
				continue;
			if (item_type_flags (packitem, accepted))
			{
				m = (struct MenuOption) {get_Itref (packitem), {0,}, NULL};
				char *line = get_near_desc (MTHIID(packitem->loc.inv.monsID), packitem);
				gr_ext (&m.ex_str[2], line);
				m.ex_str[0] = packitem->type.gl;
				m.ex_str[1] = ' ';
				free (line);
				v_push (inv, &m);
				++ num_items;
			}
		}
	}
	//if (!num_items)
	//{
	//	snprintf(first, 256, "#(empty)");
	//	v_pstr (inv, "#(empty)");
	//}
	//v_pstr (inv, "");
	m = (struct MenuOption) {0, {0,}};
	v_push (inv, &m);
	char out = p_menuex (inv);
	v_free (inv);
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

