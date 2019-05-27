/* pack.c */

#include "include/thing.h"
#include "include/pack.h"
#include "include/item.h"
#include "include/panel.h"
#include "include/dlevel.h"
#include "include/string.h"
#include "include/debug.h"
#include "include/vector.h"
#include "include/monst.h"

void pack_rem (Pack *pack, unsigned u)
{
	pack->items[u] = 0;
}

int pack_add (Pack *pack, ItemID it, int u)
{
	if (!pack->items[u])
	{
		/* Put in pack */
		pack->items[u] = it;
		return 1;
	}
	panic ("merge attempt in pack_add");
	return 0;
	//return it_merge (&pack->items[u], it);
}

void add_desc (struct String *str, Pack *pack, int itcat, MonsID mons, V_ItemID contents)
{
	if (itcat == ITCAT_HANDS)
	{
		str_catf (str, "#o#nFFF00000%c#nBBB00000 Your bare hands\n", 0xED);
		ItemID ID = 0;
		v_push (contents, &ID);
		return;
	}
	if (!pack)
		return;
	
	int i, any;
	char line[128];
	for (i = any = 0; i < MAX_ITEMS_IN_PACK; ++i)
	{
		ItemID item = pack->items[i];
		if (!item)
			continue;
		if (it_category (it_sort (item)) != itcat)
			continue;
		if (!any)
		{
			str_catf (str, "#n000BBB00%s#nBBB00000\n", item_appearance [itcat]);
			any = 1;
		}
		it_ndesc (line, 128, item, mons);
		str_catf (str, "#o%s\n", line);
		v_push (contents, &item);
	}
}

ItemID show_contents (MonsID mons, uint32_t accepted, char *msg)
{
	struct Pack *pack = mons_pack (mons);
	struct String *fmt = str_dinit ();
	V_ItemID contents = v_init (sizeof (ItemID), 52); // can be larger?
	str_catf (fmt, "#nFFF00000#c%s#nBBB00000\n\n", msg);

	int i;
	for (i = 0; i != ITCAT_END; ++ i)
	{
		if (accepted & (1 << i))
			add_desc (fmt, pack, i, mons, contents);
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
		return 0;
	}
	ItemID ret = contents->data[num];
	v_free (contents);
	return ret;
}

