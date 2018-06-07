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
			ADD_MAP(DGN_DOOR, i+1);
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
			ADD_MAP(DGN_OPENDOOR, i-2);
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
	/*Ityp is;
	memcpy (&is, &(all_items[rn(NUM_ITEMS)]), sizeof(is));
	struct Item it = {0, {.loc = LOC_NONE}, is, 0, is.wt, NULL};
	//if (is.type == ITYP_JEWEL)
	//	it.attr |= rn(NUM_JEWELS) << 16;
	struct Item *ret = malloc(sizeof(it));
	memcpy (ret, &it, sizeof(it));
	return ret;*/
	return NULL;
}

void generate_map (struct DLevel *lvl, enum LEVEL_TYPE type)
{
	int start, end;
	Vector *things = lvl->things;

	if (type == LEVEL_NORMAL)
	{
		int i;

		total_rooms = 0;
		attempt_room (lvl, map_graph->h/2 - 2 - rn(3), map_graph->w/2 - 3 - rn(5), 15, 20);
		do add_another_room (lvl);
		while (total_rooms < 100);

		start = map_buffer (map_graph->h/2, map_graph->w/2);
		/* Down-stair */
		do
			end = (int32_t) rn(map_graph->a);
		while (end == start);
		ADD_MAP(DGN_DOWNSTAIR, end);
		
		/* clear space at the beginning (for the up-stair) */
		ADD_MAP (DGN_GROUND, start);

		/* clear space for the down-stair */
		ADD_MAP (DGN_GROUND, end);

		/* fill the rest up with walls */
		for (i = 0; i < map_graph->a; ++i)
			if (things[i]->len == 0)
				ADD_MAP (DGN_WALL, i);

		//for (i = 0; i < 100; ++ i)
		//{
		//	gen_mons_in_level ();
		//	do
		//	{
		//		y = rn (map_graph->h);
	//			x = rn (map_graph->w);
	//		}
	//		while (!is_safe_gen (lvl, y, x));

//			struct Item *item = gen_item ();
			//new_thing (THING_ITEM, lvl, y, x, item);
//			free (item);
//			mons_gen (cur_dlevel, 2, U.luck-30);
		//}
		//mons_gen (cur_dlevel, 3, 0);
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
	if (lvl->monsIDs[n])
		return false;
	int i;
	for (i = 0; i < things[n]->len; ++ i)
	{
		T = THING(things, n, i);
		if (T->type == THING_DGN)
		{
			m = &(T->thing.mis);
			if (!(m->attr & 1))
				return false;
		}
	}
	return true;
}

void init_mons (struct Monster *mons, int type)
{
	memcpy (mons, &all_mons[type], sizeof(struct Monster));
	//mons->type = type;
}

/* initialised at start of game */
struct Monster *gen_player (int upsy, int upsx, char *name)
{
	struct Monster m1;
	init_mons (&m1, MTYP_human);
	m1.name = name;
	m1.skills = v_dinit (sizeof(struct Skill));
	m1.exp = 19;
	m1.ctr.mode = CTR_PL;
	m1.level = 1;
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_WATER_BOLT, 0, 1}));
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_FIREBALL, 0, 1}));
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_FROST, 0, 1}));
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_FLAMES, 0, 1}));
	struct Monster *pl = new_mons (cur_dlevel, upsy, upsx, &m1);
	struct Item *item;
	int num = rn(40)+20;
	struct Item myitem = new_items (ityps[ITYP_GOLD_PIECE], num);
	item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 0}});
	struct Item myaxe = new_item (ityps[ITYP_DAGGER]);
	item = item_put (&myaxe, (union ItemLoc) { .dlvl = {LOC_INV, pl->ID, 1}});
	mons_wield (pl, 0, item);
	struct Skill skill = {SK_USE_DAGGER, 19, 0};
	v_push (pl->skills, &skill);
	myitem = new_item (ityps[ITYP_LEATHER_HAT]);
	item = item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 2}});
	mons_wear (pl, item, offsetof (struct WoW, heads[0]));
	myitem = new_item (ityps[ITYP_CLOTH_TUNIC]);
	item = item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 3}});
	mons_wear (pl, item, offsetof (struct WoW, torsos[0]));
	myitem = new_item (ityps[ITYP_FORCE_SHARD]);
	item = item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 4}});
	/*int i;
	for (i = 4; i < 40; ++ i)
	{
		myitem = new_item (ityps[ITYP_CLOTH_TUNIC]);
		item = item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, i}});
	}*/
	/*myitem = new_item (ityps[ITYP_GLOVE]);
	item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 3}});
	myitem = new_item (ityps[ITYP_CHAIN_MAIL]);
	item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 4}});
	myitem = new_item (ityps[ITYP_HELMET]);
	item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 5}});
	myitem = new_item (ityps[ITYP_GLOVE]);
	item_put (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl->ID, 6}});*/
	v_push (cur_dlevel->playerIDs, &pl->ID);
	return pl;
}

/* start of and during level */
struct Monster *gen_mons_in_level ()
{
	int i;
	uint32_t xloc, yloc;
	for (i = 0; i < 5; ++ i)
	{
		xloc = rn(map_graph->w), yloc = rn(map_graph->h);
		if (is_safe_gen (cur_dlevel, yloc, xloc))
			break;
	}
	if (i >= 5)
		return NULL;

	struct Monster p;
	init_mons (&p, mons_gen_type ());
	if (p.mflags & FL_HOSTILE)
		p.ctr.mode = CTR_AI_HOSTILE;
	else
		p.ctr.mode = CTR_AI_TIMID;
	p.level = 1; //mons[p.type].exp? TODO
	struct Monster *th = new_mons (cur_dlevel, yloc, xloc, &p);
	//printf ("successful generation \n");
	return th;
}

struct Monster *gen_boss (int yloc, int xloc)
{
	struct Monster p;
	init_mons (&p, MTYP_dwarf);
	if (p.mflags & FL_HOSTILE)
		p.ctr.mode = CTR_AI_HOSTILE;
	else
		p.ctr.mode = CTR_AI_TIMID;
	p.level = 1; //mons[p.type].exp? TODO
	struct Monster *th = new_mons (cur_dlevel, yloc, xloc, &p);
	struct Item myaxe = new_item (ityps[ITYP_BATTLE_AXE]);
	item_put (&myaxe, (union ItemLoc) { .inv = {LOC_INV, th->ID, 0}});
	return th;
}

