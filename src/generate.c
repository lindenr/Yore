/* generate.c */

#include "include/all.h"
#include "include/generate.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/graphics.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/skills.h"
#include "include/event.h"

#include <stdio.h>
#include <assert.h>

#define NUM(y,x,s,a0,a1,a2,a3,a4,a5,a6,a7,a8)\
                  (((s)[(y-1)*xsiz + (x)-1]<<a8) +\
                   ((s)[(y  )*xsiz + (x)-1]<<a7) +\
                   ((s)[(y+1)*xsiz + (x)-1]<<a6) +\
                   ((s)[(y-1)*xsiz + (x)  ]<<a5) +\
                   ((s)[(y  )*xsiz + (x)  ]<<a4) +\
                   ((s)[(y+1)*xsiz + (x)  ]<<a3) +\
                   ((s)[(y-1)*xsiz + (x)+1]<<a2) +\
                   ((s)[(y ) *xsiz + (x)+1]<<a1) +\
                   ((s)[(y+1)*xsiz + (x)+1]<<a0))
#define ADD_MAP(type, i) new_thing (THING_DGN, lvl, (i) / map_graph->w, (i) % map_graph->w, &map_items[type])

void load_auto (double *nums)
{
	FILE *ifp = fopen("/home/lindenr/prog/Yore/SAMPLE", "r");
	if (ifp == NULL) return;

	int ysiz, xsiz;
	fscanf (ifp, "%d %d", &ysiz, &xsiz);
	//fprintf(stderr, "%d %d\n", ysiz, xsiz);
	uint16_t *s = malloc(2*ysiz * xsiz);

	int i, x, y;
	for (i = 0; i < ysiz*xsiz; ++ i)
	{
		if ((i%xsiz)==0)
			fscanf (ifp, "%c", (char*)(&(s[i])));
		fscanf (ifp, "%c", (char*)(&(s[i])));
		//printf("%c", s[i]);
		//printf("%d ", s[i]);
		s[i] = ((char)s[i] == ' ');
		//if ((i%xsiz) == xsiz-1) printf("\n");
	}
	//printf("\n");
	for (i = 0; i < (1<<9); ++ i)
		nums[i] = 0.1;

	//for (i = 0; i < ysiz*xsiz; ++ i)
	//	printf("%d%s", s[i], (((i+1)%xsiz)?" ":"\n"));
	for (y = 1; y < ysiz-1; ++ y/*, fprintf(stderr, "\n")*/) for (x = 1; x < xsiz-1; ++ x)
	{
		//fprintf(stderr, "(%d, %d): %d\n", y, x, s[y*xsiz + x]);
		nums[NUM(y,x,s,8,7,6,5,4,3,2,1,0)] += 1.0;
		nums[NUM(y,x,s,6,7,8,3,4,5,0,1,2)] += 1.0;
		nums[NUM(y,x,s,2,1,0,5,4,3,8,7,6)] += 1.0;
		nums[NUM(y,x,s,0,1,2,3,4,5,6,7,8)] += 1.0;
		nums[NUM(y,x,s,6,3,0,7,4,1,8,5,2)] += 1.0;
		nums[NUM(y,x,s,0,3,6,1,4,7,2,5,8)] += 1.0;
		nums[NUM(y,x,s,2,5,8,1,4,7,0,3,6)] += 1.0;
		nums[NUM(y,x,s,8,5,2,7,4,1,6,3,0)] += 1.0;
	}
	fclose(ifp);
	free(s);
}

/* each element of output[] should be 0 or 1 */
void generate_auto (double *nums, int yout, int xout, uint16_t *output, int a_start, int a_end, double anneal)
{
	//srand(5);
	int x, y;
	//for (i = 0; i < xout*yout; ++ i)
	//	output[i] = !!(rand()%5);
	y = 2; x = 2; int a = 1;
	int xsiz = xout;
	for (a = a_start; a >= a_end; -- a)
	{
	while (1)
	{
		double n1 = nums[NUM(y-1,x-1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x-1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y+1,x-1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y-1,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y+1,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y-1,x+1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x+1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y+1,x+1,output,8,7,6,5,4,3,2,1,0)];
		output[y*xsiz + x] = !output[y*xsiz + x];
		double n2 = nums[NUM(y-1,x-1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x-1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y+1,x-1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y-1,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y+1,x  ,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y-1,x+1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y  ,x+1,output,8,7,6,5,4,3,2,1,0)] *
		         nums[NUM(y+1,x+1,output,8,7,6,5,4,3,2,1,0)];
		//double tmp = ((double)a)/5;
		//(((double)(rand()%1000))/1000.0) < (n2/n1)
		if (n2 < n1/2+(rand()%(1+(int)((((double)a)/anneal)*n2 + n1/2))))
		//if (n1>n2 || (!(rand()%4)))
		//if (n1 > n2)
			output[y*xsiz + x] = !output[y*xsiz + x];
	//	else if (n2 > 10000000000000000.0)
	//	{
	//		fprintf(stderr, "%lf %lf\n", n1, n2);
	//	}
		//else if (a > 998) printf("%d ", a);
		x += 11; x = ((x-2)%(xout-4)) + 2;
		if (x == 2) {y += 11; y = ((y-2)%(yout-4)) + 2;}
		if (x == 2 && y == 2) break;
	}
	}
	//	printf("\n");
	/*for (y = 2; y < yout-2; ++ y) {for (x = 2; x < xout-2; ++ x){
		if (output[y*xsiz+x])
		{
		//ADD_MAP(DGN_GROUND, map_buffer(y,x));
		ADD_MAP(DGN_GRASS2, map_buffer(2*y,  2*x));
		ADD_MAP(DGN_GRASS2, map_buffer(2*y+1,2*x));
		ADD_MAP(DGN_GRASS2, map_buffer(2*y,  2*x+1));
		ADD_MAP(DGN_GRASS2, map_buffer(2*y+1,2*x+1));
		/ *ADD_MAP(DGN_WALL, map_buffer(3*y,3*x));
		ADD_MAP(DGN_WALL, map_buffer(3*y+1,3*x));
		ADD_MAP(DGN_WALL, map_buffer(3*y+2,3*x));
		ADD_MAP(DGN_WALL, map_buffer(3*y,3*x+1));
		ADD_MAP(DGN_WALL, map_buffer(3*y+1,3*x+1));
		ADD_MAP(DGN_WALL, map_buffer(3*y+2,3*x+1));
		ADD_MAP(DGN_WALL, map_buffer(3*y,3*x+2));
		ADD_MAP(DGN_WALL, map_buffer(3*y+1,3*x+2));
		ADD_MAP(DGN_WALL, map_buffer(3*y+2,3*x+2));* /
		}
		//	printf("#");
		//else printf(" ");
		}
		//printf("\n");
	}*/
}


bool check_area (struct DLevel *lvl, int y, int x, int ys, int xs)
{
	Vector *things = lvl->things;
	int i, j, k;
	if (y < 0 || y + ys >= map_graph->h ||
		x < 0 || x + xs >= map_graph->w)
		return false;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = map_buffer (y+j, x+k);
			if (things[i]->len != 0) return false;
			-- j;
		}
		-- k;
	}
	return true;
}

int total_rooms = 0;
bool attempt_room (struct DLevel *lvl, int y, int x, int ys, int xs)
{
	int i, j, k;
	if (!check_area (lvl, y-2, x-2, ys+4, xs+4)) return false;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = map_buffer (y+j, x+k);
			ADD_MAP (DGN_GROUND, i);
			-- j;
		}
		-- k;
	}
	++ total_rooms;
	return true;
}

void add_another_room (struct DLevel *lvl)
{
	Vector *things = lvl->things;
	int i;

	do
		i = rn(map_graph->a);
	while (things[i]->len == 0);

	if (things[i+1]->len == 0)
	{
		int x = (i+1)%map_graph->w, y = (i+1)/map_graph->w;
		if (attempt_room (lvl, y - 2 - rn(3), x + 1, 6 + rn(3), 6))
		{
			ADD_MAP(DGN_GROUND, i+1);
			ADD_MAP(DGN_GROUND, i+2);
		}
	}
	else if (things[i-1]->len == 0)
	{
		int x = (i-1)%map_graph->w, y = (i-1)/map_graph->w;
		if (attempt_room (lvl, y - 2 - rn(3), x - 8, 6 + rn(3), 6))
		{
			ADD_MAP(DGN_GROUND, i-1);
			ADD_MAP(DGN_GROUND, i-2);
		}
	}
	else if (things[i-map_graph->w]->len == 0)
	{
		int x = (i-map_graph->w)%map_graph->w, y = (i-map_graph->w)/map_graph->w;
		if (attempt_room (lvl, y - 8, x - 3 - rn(5), 6, 8 + rn(5)))
		{
			ADD_MAP(DGN_GROUND, i-map_graph->w);
			ADD_MAP(DGN_GROUND, i-map_graph->w*2);
		}
	}
	else if (things[i+map_graph->w]->len == 0)
	{
		int x = (i+map_graph->w)%map_graph->w, y = (i+map_graph->w)/map_graph->w;
		if (attempt_room (lvl, y + 1, x - 3 - rn(5), 6, 8 + rn(5)))
		{
			ADD_MAP(DGN_GROUND, i+map_graph->w);
			ADD_MAP(DGN_GROUND, i+map_graph->w*2);
		}
	}
}

struct Item *gen_item ()
{
	ityp is;
	memcpy (&is, &(items[rn(NUM_ITEMS)]), sizeof(is));
	struct Item it = {is, 0, is.wt, NULL};
	//if (is.type == ITYP_JEWEL)
	//	it.attr |= rn(NUM_JEWELS) << 16;
	struct Item *ret = malloc(sizeof(it));
	memcpy (ret, &it, sizeof(it));
	return ret;
}

void generate_map (struct DLevel *lvl, enum LEVEL_TYPE type)
{
	int start, end;
	Vector *things = lvl->things;

	if (type == LEVEL_NORMAL)
	{
		int i, y, x;

		total_rooms = 0;
		attempt_room (lvl, map_graph->h/2 - 2 - rn(3), map_graph->w/2 - 3 - rn(5), 15, 20);
		do add_another_room (lvl);
		while (total_rooms < 100);

		start = map_buffer (map_graph->h/2, map_graph->w/2);
		end = mons_gen (lvl, 1, start);
		
		/* clear space at the beginning (for the up-stair) */
		ADD_MAP (DGN_GROUND, start);

		/* clear space for the down-stair */
		ADD_MAP (DGN_GROUND, end);

		/* fill the rest up with walls */
		for (i = 0; i < map_graph->a; ++i)
			if (things[i]->len == 0)
				ADD_MAP (DGN_WALL, i);

		for (i = 0; i < 100; ++ i)
		{
			do
			{
				y = rn (map_graph->h);
				x = rn (map_graph->w);
			}
			while (!is_safe_gen (lvl, y, x));

			struct Item *item = gen_item ();
			new_thing (THING_ITEM, lvl, y, x, item);
			free (item);
		}
	}
	else if (type == LEVEL_TOWN)
	{
		uint16_t *out10x30, *out20x60, *out100x300;
		int i, x, y;
		out10x30 = malloc(2*10*30);
		out20x60 = malloc(2*20*60);
		out100x300 = malloc(2*100*300);
		double nums[512];
		load_auto (nums);
		//for (i = 0; i < 512; ++ i)
		//	if (nums[i]>3.0)
		//		printf("%d,%d ", i, (int)nums[i]);
		//srand(5);
	
		for (y = 0; y < 10; ++ y) for (x = 0; x < 30; ++ x)
			out10x30[y*30 + x] = rand()%2;
		generate_auto(nums, 10, 30, out10x30, 1000, 30, 40.0);
		for (y = 0; y < 20; ++ y) for (x = 0; x < 60; ++ x)
			out20x60[y*60 + x] = out10x30[(y/2)*30 + x/2];
		generate_auto(nums, 20, 60, out20x60, 20, 10, 10.0);
		for (y = 0; y < 100; ++ y) for (x = 0; x < 300; ++ x)
			out100x300[y*300 + x] = out20x60[(y/5)*60 + x/5];
		generate_auto(nums, 100, 300, out100x300, 99, 98, 100.0);
		for (i = 0; i < map_graph->a; ++i) {
			if (out100x300[i])
				ADD_MAP (DGN_GRASS1, i);
			else
				ADD_MAP (DGN_GRASS2, i);

			if (rand()%100){}
			else if ((rand()%2))
				ADD_MAP (DGN_FLOWER2, i);
			else if ((rand()%2))
				ADD_MAP (DGN_FLOWER1, i);
			else
				ADD_MAP (DGN_TREE, i);
		}
		free(out10x30);
		free(out20x60);
		free(out100x300);
	}
	else if (type == LEVEL_MAZE)
	{
		/* TODO */
	}
}

/* can a monster be generated here? (no monsters or walls in the way) */
bool is_safe_gen (struct DLevel *lvl, uint32_t yloc, uint32_t xloc)
{
	Vector *things = lvl->things;
	struct Thing *T;
	struct map_item_struct *m;
	int n = map_buffer(yloc, xloc);
	LOOP_THING(things, n, i)
	{
		T = THING(things, n, i);
		if (T->type == THING_MONS)
			return false;
		if (T->type == THING_DGN)
		{
			m = &(T->thing.mis);
			if (!(m->attr & 1))
				return false;
		}
	}
	return true;
}

char *real_player_name;

/* type:
 * 0 : initialised at start of game
 * 1 : generated at start of level
 * 2 : randomly throughout level */
uint32_t mons_gen (struct DLevel *lvl, int type, int32_t param)
{
	int32_t luck, start;
	int32_t end;
	uint32_t upsy, upsx;
	if (type == 0)
	{
		start = param;
		upsy = start / map_graph->w;
		upsx = start % map_graph->w;

		struct Monster m1 = {MTYP_HUMAN, {.mode=AI_NONE}, 1, 0, 20, 20, 0.0, 10, 10, 0.0, 0,};//1000, 0, 0, {{0},}, {0,}, {{0,},}, NULL, 0, NULL};
		m1.name = "Thing 1";
		m1.skills = v_dinit (sizeof(struct Skill));
		m1.speed = all_mons[m1.type].speed;
		v_push (m1.skills, (const void *)(&(const struct Skill) {SK_CHARGE, 0, 1}));
		//v_push (m1.skills, (const void *)(&(const struct Skill) {SK_DODGE, 0, 1}));
		struct Thing *t1 = new_thing (THING_MONS, lvl, upsy, upsx, &m1);
		ev_queue (1, (union Event) { .mturn = {EV_MTURN, t1->ID}});
		ev_queue (1, (union Event) { .mregen = {EV_MREGEN, t1->ID}});

		/*struct Monster m2 = {MTYP_HUMAN, CTRL_PLAYER, 1, 0, 20, 20, 0.0, 10, 10, 0.0, 1000, 0, 0, {{0},}, {0,}, 0, 0, 0, NULL};
		m2.name = "Thing 2";
		m2.skills = v_dinit (sizeof(struct Skill));
		//v_push (m2.skills, (const void *)(&(const struct Skill) {SK_CHARGE, 0, 1}));
		//v_push (m2.skills, (const void *)(&(const struct Skill) {SK_DODGE, 0, 1}));
		struct Thing *t2 = new_thing (THING_MONS, lvl, upsy, upsx+1, &m2);
		ev_queue (1, (union Event) { .mturn = {EV_MTURN, t2->ID}});
		ev_queue (1, (union Event) { .mregen = {EV_MREGEN, t2->ID}});*/
	}
	else if (type == 1)
	{
		/* Up-stair */
		start = param;
		ADD_MAP(DGN_UPSTAIR, start);

		/* Down-stair */
		do
			end = (int32_t) rn(map_graph->a);
		while (end == start);
		ADD_MAP(DGN_DOWNSTAIR, end);

		/* Move to the up-stair */
//		thing_bmove (player, start);
		return end;
	}
	else if (type == 2)
	{
		luck = param;
		if (rn(100) >= (uint32_t) (15 - 2*luck))
			return 0;

		uint32_t xloc = rn(map_graph->w), yloc = rn(map_graph->h);
		if (!is_safe_gen (lvl, yloc, xloc))
			return 0;

		struct Monster p;
		memclr (&p, sizeof(p));
		p.type = player_gen_type ();
		p.ai.mode = AI_TIMID;
		p.HP = (all_mons[p.type].flags >> 28) + (all_mons[p.type].exp >> 1);
		p.HP += 1+rn(1+ p.HP / 3);
		p.HP_max = p.HP;
		p.ST = 10;
		p.ST_max = p.ST;
		p.speed = all_mons[p.type].speed;
		p.name = NULL;
		p.level = 1; //mons[p.type].exp? TODO
		p.exp = all_mons[p.type].exp;
		struct Thing *th = new_thing (THING_MONS, lvl, yloc, xloc, &p);
		ev_queue (1, (union Event) { .mturn = {EV_MTURN, th->ID}});
		//printf ("successful generation \n");
	}
	return 0;
}

