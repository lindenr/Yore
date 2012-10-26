/* list.c */

#include "include/all.h"
#include "include/list.h"
#include "include/graphics.h"

struct list_iter list_beg = {0, 0, 0},
                 list_end = {0, 0, 0};

extern struct List all_things[MAP_HEIGHT*MAP_WIDTH];

void list_free(struct List *list)
{
	struct list_iter *i;
	for (i = list->beg; iter_good(i); next_iter(&i))
	{
		if (i != list->beg)
			free(i->prev);
	}
	if (i != list->beg)
		free(i->prev);
}

struct list_iter *next_iter(struct list_iter **li)
{
	if (!is_valid(*li))
	{
		**li = list_end;
		return NULL;
	}
	/* (*li)->next is valid */
	*li = (*li)->next;
	return (*li);
}

bool iter_equal(struct list_iter li1, struct list_iter li2)
{
	return ((li1.data == li2.data) &&
			(li1.prev == li2.prev) && (li1.next == li2.next));
}

void push_back(struct List *list, void *data)
{
	struct list_iter *li = malloc(sizeof(struct list_iter));
	if (!is_valid(list->end))	/* no list */
	{
		list->beg = li;
		li->prev = &list_beg;
	}
	else
	{
		li->prev = list->end;
		list->end->next = li;
	}
	li->data = data;
	list->end = li;
	li->next = &list_end;
}

void list_rem(struct List *list, struct list_iter *li)
{
	struct list_iter *prev = li->prev, *next = li->next;
	if ((!is_valid(next)) && is_valid(prev))
	{
		prev->next = &list_end;
		list->end = prev;
	}
	else if ((!is_valid(prev)) && is_valid(next))
	{
		next->prev = &list_beg;
		list->beg = next;
	}
	else if ((!is_valid(prev)) && (!is_valid(next)))
	{
		list->beg = &list_beg;
		list->end = &list_end;
	}
	else
	{
		prev->next = next;
		next->prev = prev;
	}
	li->next = 0;
	li->prev = 0;
	li->data = 0;
}

bool is_valid(struct list_iter *li)
{
	return !(li->next == 0 && li->prev == 0 && li->data == 0);
}

bool iter_good(struct list_iter * li)
{
	if (!li)
		return false;
	return is_valid(li);
}
