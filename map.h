#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <stdint.h>

struct map_item_struct
{
    char name[20];
    char ch;
    uint32_t attr;
};

extern struct map_item_struct map_items[];
extern int             GETMAPITEMID(char);

#endif /* MAP_H_INCLUDED */
