/* pack.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/pack.h"
#include "include/item.h"
#include "include/panel.h"
#include "include/dlevel.h"
#include "include/string.h"

Pack *pack_init ()
{
	Pack *pack = malloc(sizeof(Pack));
	memset (pack, 0, sizeof(Pack));
	return pack;
}

struct Item *pack_rem (Pack *pack, unsigned u)
{
	struct Item *ret = &pack->items[u];
	ret->type.type = ITSORT_NONE;
	return ret;
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
	return it_merge (&pack->items[u], it);
}

void add_desc (struct String *str, Pack *pack, int itcat, struct Monster *mons, Vector contents)
{
	if (itcat == ITCAT_HANDS)
	{
		str_catf (str, "#o#nFFF00000###nBBB00000 Your bare hands\n");
		TID ID = 0;
		v_push (contents, &ID);
		return;
	}
	if (!pack)
		return;
	
	int i, any;
	char line[128];
	for (i = any = 0; i < MAX_ITEMS_IN_PACK; ++i)
	{
		struct Item *packitem = &pack->items[i];
		if (NO_ITEM(packitem))
			continue;
		if (it_category (packitem->type.type) != itcat)
			continue;
		if (!any)
		{
			str_catf (str, "#n000BBB00%s#nBBB00000\n", item_appearance [itcat]);
			any = 1;
		}
		it_desc (line, packitem, mons);
		str_catf (str, "#o%s\n", line);
		v_push (contents, &packitem->ID);
	}
}

TID show_contents (struct Monster *mons, uint32_t accepted, char *msg)
{
	struct Pack *pack = mons->pack;
	struct String *fmt = str_dinit ();
	Vector contents = v_init (sizeof (TID), 52); // can be larger?
	str_catf (fmt, "#nFFF00000#c%s#nBBB00000\n\n", msg);

	int i;
	for (i = 0; it_displayorder[i] != -1; ++ i)
	{
		if (accepted & (1 << it_displayorder[i]))
			add_desc (fmt, pack, it_displayorder[i], mons, contents);
	}
	if (contents->len == 0)
	{
		str_free (fmt);
		v_free (contents);
		p_msg ("No items to show.");
		return -1;
	}
	int num = p_flines (str_data (fmt));
	str_free (fmt);
	if (num == -1)
	{
		v_free (contents);
		return -1;
	}
	TID ret = *(TID*)v_at (contents, num);
	v_free (contents);
	return ret;
}

/*int item_type_flags (struct Item *item, uint32_t accepted)
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
		case 0xFB:
			return (accepted & ITCAT_JEWEL);
	}
	panic ("item_type_flags() found a strange item type.");
	return -1;
}*/

