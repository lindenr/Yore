/* generate.c
 * Linden Ralph */

#include "all.h"
#include "generate.h"
#include "rand.h"
#include "thing.h"
#include "map.h"

int WALL_TYPE(int y, int u, int h, int j, int k, int l, int b, int n)
{
    if (h == DOT || h == ' ')
    {
        if (k == DOT || k == ' ')
        {
            if (l == DOT || l == ' ')
            {
                if (j == DOT || j == ' ') return DOT;
                else          return ACS_VLINE;
            }
            else
            {
                if (j == DOT || j == ' ') return ACS_HLINE;
                else          return ACS_ULCORNER;
            }
        }
        else
        {
            if (l == DOT || l == ' ')     return ACS_VLINE;
            else
            {
                if (j == DOT || j == ' ') return ACS_LLCORNER;
                else          return ACS_LTEE;
            }
        }
    }
    else
    {
        if (k == DOT || k == ' ')
        {
            if (l == DOT || l == ' ')
            {
                if (j == DOT || j == ' ') return ACS_HLINE;
                else          return ACS_URCORNER;
            }
            else
            {
                if (j == DOT || j == ' ') return ACS_HLINE;
                else          return ACS_TTEE;
            }
        }
        else
        {
            if (l == DOT || l == ' ')
            {
                if (j == DOT || j == ' ') return ACS_LRCORNER;
                else          return ACS_RTEE;
            }
            else
            {
                if (j == DOT || j == ' ') return ACS_BTEE;
                else
                {
                    if (y == DOT || u == DOT || b == DOT || n == DOT)
                              return ACS_PLUS;
                    else      return 'D';
                }
            }
        }
    }
}

int is_wall (int y, int u, int h, int j, int k, int l, int b, int n)
{
    if ((y != DOT) && (u != DOT) && (h != DOT) && (j != DOT)
        && (k != DOT) && (l != DOT) && (b != DOT) && (n != DOT))
        return ' ';
    return 'W';
}

void generate_map(enum LEVEL_TYPE type)
{
	int i, x, Y;
    int buffer[1680];
    for (i = 0; i < 1680; ++ i)
    {
        buffer[i] = ' ';
    }
    /* slightly better :/ */
    if (type == LEVEL_MINES)
    {
        int t = 200;
        while (t--) buffer[even_prob(&RNG_main, 1520)+79]=DOT;

        t = 800;
        while (t--)
        {
            int buf = even_prob(&RNG_main, 1520)+79;

            /* printw("%d\n", buf);refresh(); */
            if ((buffer[buf] != DOT) && ((buffer[buf+1]==DOT)||(buffer[buf-1]==DOT)
                                         ||(buffer[buf+80]==DOT)||(buffer[buf-80]==DOT)))
                buffer[buf]=DOT;
            else
				t++;
        }
        for (i = 0; i < 1680; ++ i)
            if (80<=i && i<1600)if(buffer[i] != DOT) buffer[i] = 'W';
        for (x = 0; x < 80; ++ x)
        {
            for (Y = 0; Y < 21; ++ Y)
            {
                bool both = true;
                int y=DOT,u=DOT,h=DOT,j=DOT,k=DOT,l=DOT,b=DOT,n=DOT;
                int buf = x + 80*Y;
                if (buffer[buf] == DOT) continue;
                if (x == 0){       y = h = b = DOT; both = false;}
                else if (x == 79){ u = l = n = DOT; both = false;}
                /* x-middle */
                else
                {
                    h = buffer[buf-1];
                    l = buffer[buf+1];
                }
                if (Y == 0){       y = k = u = DOT; both = false;}
                else if (Y == 20){ b = j = n = DOT; both = false;}
                /* y-middle */
                else
                {
                    k = buffer[buf-80];
                    j = buffer[buf+80];
                }
                if(both)
                {
                    y = buffer[buf-81];
                    u = buffer[buf-79];
                    b = buffer[buf+79];
                    n = buffer[buf+81];
                }
                buffer[buf] = is_wall(y,u,h,j,k,l,b,n);
            }
        }
        for (x = 0; x < 80; ++ x)
        {
            for (Y = 0; Y < 21; ++ Y)
            {
                int both = true;
                int y=DOT,u=DOT,h=DOT,j=DOT,k=DOT,l=DOT,b=DOT,n=DOT;
                int buf = x + 80*Y;
                if (buffer[buf] != 'W') continue;
                if (x == 0)      { y = h = b = DOT; both = false;}
                else if (x == 79){ u = l = n = DOT; both = false;}
                /* x-middle */
                else
                {
                    h = buffer[buf-1];
                    l = buffer[buf+1];
                }
                if (Y == 0)      { y = k = u = DOT; both = false;}
                else if (Y == 20){ b = j = n = DOT; both = false;}
                /* y-middle */
                else
                {
                    k = buffer[buf-80];
                    j = buffer[buf+80];
                }
                if (both)
                {
                    y = buffer[buf-81];
                    u = buffer[buf-79];
                    b = buffer[buf+79];
                    n = buffer[buf+81];
                }
                buffer[buf] = WALL_TYPE(y,u,h,j,k,l,b,n);
            }
        }
       /* move(1,0); */
       /* for(int i = 0; i < 2000; ++ i) addch(buffer[i]); */
		for(i = 0; i < 1680; ++ i)
		{
			struct map_item_struct *mis = malloc(sizeof(struct map_item_struct));
			memcpy(mis, &(map_items[GETMAPITEMID(buffer[i])]), sizeof(struct map_item_struct));
			new_thing(THING_DGN, i/80, i%80, mis);
		}
    }
    else if (type == LEVEL_NORMAL)
    {

    }
    else if (type == LEVEL_FOREST)
	{/*
		struct List lis = get_level_as_list("level_forest");
		struct list_iter *li;

		for (li = lis.beg; iter_good(li); next_iter(&li))
		{
			
		}*/ /* Do all that later */
	}
}
