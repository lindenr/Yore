/* generate.c */

#include "include/generate.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/graphics.h"
#include "include/vector.h"
#include "include/dlevel.h"
#include "include/skills.h"
#include "include/event.h"
#include "include/item.h"
#include "include/world.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

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
		//dlv_settile (dlevel, DGN_GROUND, map_buffer(y,x));
		dlv_settile (dlevel, DGN_GRASS2, map_buffer(2*y,  2*x));
		dlv_settile (dlevel, DGN_GRASS2, map_buffer(2*y+1,2*x));
		dlv_settile (dlevel, DGN_GRASS2, map_buffer(2*y,  2*x+1));
		dlv_settile (dlevel, DGN_GRASS2, map_buffer(2*y+1,2*x+1));
		/ *dlv_settile (dlevel, DGN_WALL, map_buffer(3*y,3*x));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y+1,3*x));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y+2,3*x));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y,3*x+1));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y+1,3*x+1));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y+2,3*x+1));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y,3*x+2));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y+1,3*x+2));
		dlv_settile (dlevel, DGN_WALL, map_buffer(3*y+2,3*x+2));* /
		}
		//	printf("#");
		//else printf(" ");
		}
		//printf("\n");
	}*/
}

static char *cur_gen = NULL;
static int gen_w, gen_h, gen_t, gen_a;
#define IDX(y,x) ((y)*gen_w + x)
int check_area (int y, int x, int ys, int xs)
{
	int i, j, k;
	if (y < 0 || y + ys >= gen_h ||
		x < 0 || x + xs >= gen_w)
		return 0;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = IDX (y+j, x+k);
			if (cur_gen[i] == ACS_BIGDOT) return 0;
			-- j;
		}
		-- k;
	}
	return 1;
}

int total_rooms = 0;
int attempt_room (int y, int x, int ys, int xs)
{
	int i, j, k;
	if (!check_area (y-2, x-2, ys+4, xs+4)) return 0;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = IDX (y+j, x+k);
			cur_gen[i] = ACS_BIGDOT;
			-- j;
		}
		-- k;
	}
	for (k = 0; k <= xs+1; ++ k)
	{
		cur_gen[IDX (y, x+k)] = ACS_WALL;
		cur_gen[IDX (y+ys+1, x+k)] = ACS_WALL;
	}
	for (j = 0; j <= ys+1; ++ j)
	{
		cur_gen[IDX (y+j, x)] = ACS_WALL;
		cur_gen[IDX (y+j, x+xs+1)] = ACS_WALL;
	}
	++ total_rooms;
	return 1;
}

void add_another_room ()
{
	int i;

	do
		i = rn(gen_a);
	while (cur_gen[i] != ACS_BIGDOT);

	if (cur_gen[i+1] != ACS_BIGDOT)
	{
		int x = (i+1)%gen_w, y = (i+1)/gen_w;
		if (attempt_room (y +1- 2 - rn(3), x + 4, 6 + rn(3), 6))
		{
			cur_gen[i+1] = ACS_BIGDOT;
		//	cur_gen[i+1] = ACS_BIGDOT;
			cur_gen[i+2] = ACS_CORRIDOR;
			cur_gen[i+3] = ACS_CORRIDOR;
			cur_gen[i+3+gen_w] = ACS_CORRIDOR;
			cur_gen[i+4+gen_w] = ACS_CORRIDOR;
			/*cur_gen[i+2-map_graph->w] = ACS_BIGDOT;
			///cur_gen[i+3-map_graph->w] = ACS_BIGDOT;
			//cur_gen[i+4-map_graph->w] = ACS_BIGDOT;
			cur_gen[i+4] = ACS_BIGDOT;
			cur_gen[i+2+map_graph->w] = ACS_BIGDOT;
			cur_gen[i+3+2*map_graph->w] = ACS_BIGDOT;
			cur_gen[i+4+2*map_graph->w] = ACS_BIGDOT;*/
			cur_gen[i+5+gen_w] = ACS_BIGDOT;
		}
	}
	else if (cur_gen[i-1] != ACS_BIGDOT)
	{
		int x = (i-1)%gen_w, y = (i-1)/gen_w;
		if (attempt_room (y - 2 - rn(3), x - 8, 6 + rn(3), 6))
		{
			cur_gen[i-1] = ACS_BIGDOT;
			cur_gen[i-2] = ACS_BIGDOT;
		//	cur_gen[i-2] = ACS_BIGDOT;
		}
	}
	else if (cur_gen[i-gen_w] != ACS_BIGDOT)
	{
		int x = (i-gen_w)%gen_w, y = (i-gen_w)/gen_w;
		if (attempt_room (y - 8, x - 3 - rn(5), 6, 8 + rn(5)))
		{
			cur_gen[i-gen_w] = ACS_BIGDOT;
			cur_gen[i-gen_w*2] = ACS_BIGDOT;
		}
	}
	else if (cur_gen[i+gen_w] != ACS_BIGDOT)
	{
		int x = (i+gen_w)%gen_w, y = (i+gen_w)/gen_w;
		if (attempt_room (y + 1, x - 3 - rn(5), 6, 8 + rn(5)))
		{
			cur_gen[i+gen_w] = ACS_BIGDOT;
			cur_gen[i+gen_w*2] = ACS_BIGDOT;
		}
	}
}

struct Item *gen_item ()
{
	/*Ityp is;
	is = ityps[rn(NUM_ITEMS)];
	struct Item it = {0, {.loc = LOC_NONE}, is, 0, is.wt, NULL};
	//if (is.type == ITYP_JEWEL)
	//	it.attr |= rn(NUM_JEWELS) << 16;
	struct Item *ret = malloc(sizeof(it));
	*ret = it;
	return ret;*/
	return NULL;
}

void gen_room (char *out, int z, int y, int x, int t, int h, int w)
{
	/*int i, j;
	struct DLevel *lvl = cur_dlevel;
	for (j = 0; j < t; ++ j) for (i = 0; i < h; ++ i)
	{
		out[dlv_index (dlevel, z+j, y+i, x)] = '#';
		out[dlv_index (dlevel, z+j, y+i, x+w-1)] = '#';
	}
	for (j = 0; j < t; ++ j) for (i = 1; i < w-1; ++ i)
	{
		out[dlv_index (dlevel, z+j, y, x+i)] = '#';
		out[dlv_index (dlevel, z+j, y+h-1, x+i)] = '#';
	}
	for (j = 1; j < h-1; ++ j) for (i = 1; i < w-1; ++ i)
	{
		out[dlv_index (dlevel, z+t-1, y+j, x+i)] = '#';
		//out[dlv_index (dlevel, z+t, y+j, x+i)] = '.';
	}*/
}

#define OUT(z,y,x) out[dlv_index (lvl, (z), (y), (x))]

void generate_map (int dlevel, enum LEVEL_TYPE type)
{
	gen_h = 50; gen_w = 150; gen_t = 11;
	gen_a = gen_h * gen_w;

	if (type == LEVEL_NORMAL)
	{
		int i;
		cur_gen = malloc (gen_a);
		for (i = 0; i < gen_a; ++ i)
			cur_gen[i] = 0;

		total_rooms = 0;
		attempt_room (gen_h/2 - 2 - rn(3), gen_w/2 - 3 - rn(5), 15, 20);
		do add_another_room ();
		while (total_rooms < 50);

		for (i = 0; i < gen_a; ++i)
		{
			int z = i/gen_a, y = (i%gen_a)/gen_w, x = i%gen_w;
			if (cur_gen[i] == ACS_BIGDOT)
			{
				dlv_settile (dlevel, z, y, x, DGN_GROUND);
				//if (!rn(5))
				//	dlv_settile (dlevel, DGN_GROUND, i + lvl->a);
				//else
					dlv_settile (dlevel, z+1, y, x, DGN_AIR);
			}
			else if (cur_gen[i] == ACS_CORRIDOR)
			{
				dlv_settile (dlevel, z, y, x, DGN_CORRIDOR);
				dlv_settile (dlevel, z+1, y, x, DGN_AIR);
			}
			else
			{
				dlv_settile (dlevel, z, y, x, DGN_AIR);
				dlv_settile (dlevel, z+1, y, x, DGN_WALL);
			}
		}
		free (cur_gen);
	}
	else if (type == LEVEL_TOWN)
	{
		/*uint16_t *out10x30, *out20x60, *out100x300;
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
				dlv_settile (dlevel, DGN_GRASS1, i);
			else
				dlv_settile (dlevel, DGN_GRASS2, i);

			if (rand()%100){}
			else if ((rand()%2))
				dlv_settile (dlevel, DGN_FLOWER2, i);
			else if ((rand()%2))
				dlv_settile (dlevel, DGN_FLOWER1, i);
			else
				dlv_settile (dlevel, DGN_TREE, i);
		}
		free(out10x30);
		free(out20x60);
		free(out100x300);*/
	}
	else if (type == LEVEL_3D)
	{
#if 1
		int z, y, x;
		for (z = 0; z < gen_t; ++ z) for (y = 0; y < gen_h; ++ y) for (x = 0; x < gen_w; ++ x)
		{
			/*if ((x/2 - 8 < z && z <= x/2 - 6) && 10 <= y && y <= 15 && z <= 9)
				dlv_settile (dlevel, DGN_ROCK, i);
			else if (z == 0 || (z == 9 && (10 > y || y > 15)))
				dlv_settile (dlevel, !rn(5), i);
			else if (z == 8 && (10 > y || y > 15))
				dlv_settile (dlevel, DGN_ROCK, i);
			else if (z == x/2 - 5 && 10 <= y && y <= 15)
				dlv_settile (dlevel, rn(3), i);*/
			double Y = (y+2.0*sin(0.1*x + sin(0.2*x)) + 3.0*sin(0.05*y));
			//Y += 200.0;
			double X = x;
			double fudge = 1.0;
			Y *= fudge;
			X *= fudge;
			int h = 1.2*((1.0+sin(0.2 * X + sin(0.15*X))) * (1.0+sin(0.2 * Y)) +
			(2.1+sin(0.125 * X+0.11*Y)));
			if (z <= 1 && h == 1)
				dlv_settile (dlevel, z, y, x, DGN_WATER);
			else if (z < h)
				dlv_settile (dlevel, z, y, x, DGN_ROCK);
			else if (z == h)
			{
				if (z >= 7)
					dlv_settile (dlevel, z, y, x, DGN_ICE);
				else
					dlv_settile (dlevel, z, y, x, DGN_GRASS1);
			}
			else
				dlv_settile (dlevel, z, y, x, DGN_AIR);
		}
#else
		char *out = malloc (lvl->v);
		int i;
		for (int i = 0; i < lvl->v; ++ i)
			out[i] = i<lvl->a?'.':0;
		gen_room (out, 0, 0, 0, 3, 8, 8);
		OUT(0, 7, 5) = '.';
		OUT(1, 7, 5) = 0;
		gen_room (out, 0, 0, 7, 3, 11, 8);
		OUT(0, 9, 7) = '.';
		OUT(1, 9, 7) = 0;
		gen_room (out, 0, 20, 0, 3, 15, 7);
		gen_room (out, 0, 20, 6, 3, 6, 7);
		gen_room (out, 0, 29, 6, 3, 6, 7);
		gen_room (out, 0, 34, 0, 3, 3, 40);
		gen_room (out, 0, 20, 12, 3, 15, 28);
		gen_room (out, 0, 15, 39, 3, 22, 10);
		gen_room (out, 0, 36, 0, 3, 8, 17);
		gen_room (out, 0, 36, 16, 3, 8, 17);
		gen_room (out, 0, 36, 32, 3, 8, 17);
		OUT(0, 8, 4) = '#';
		OUT(0, 8, 3) = '#';
		OUT(1, 8, 3) = '#';
		OUT(1, 8, 2) = '#';
		OUT(2, 8, 2) = '#';
		OUT(2, 8, 1) = '#';
		for (i = 0; i < lvl->v; ++ i)
		{
			if (out[i] == '#')
				dlv_settile (lvl, DGN_WALL+rn(3), i);
			else if (out[i] == 0)
				dlv_settile (dlevel, DGN_AIR, i);
			else if (out[i] == '.')
				dlv_settile (dlevel, DGN_GROUND, i);
		}
		free (out);
#endif
	}
	else if (type == LEVEL_MAZE)
	{
		/* TODO */
	}
	else if (type == LEVEL_SIM)
	{
		/* TODO? */
	}
}

/* can a monster be generated here? (no monsters or walls in the way) */
int is_safe_gen (int dlevel, int z, int y, int x)
{
	if (dlv_mons (dlevel, z, y, x))
		return 0;
	return dlv_passable (dlevel, z, y, x) && !dlv_passable (dlevel, z-1, y, x);
}

void init_mons (struct Monster_internal *mons, enum MTYPE type)
{
	*mons = mons_types[type];
	mons->mtype = type;
}

/* initialised at start of game */
MonsID gen_player (int dlevel, int zloc, int yloc, int xloc, char *name)
{
	struct Monster_internal m1;
	init_mons (&m1, MTYP_human);
	m1.name = name;
	m1.skills = v_dinit (sizeof(struct Skill));
	m1.exp = 0;
	m1.ctr.mode = CTR_PL;
	m1.level = 1;
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_WATER_BOLT, 0, 1}));
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_FIREBALL, 0, 1}));
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_FROST, 0, 1}));
	v_push (m1.skills, (const void *)(&(const struct Skill) {SK_FLASH, 0, 1}));
	MonsID pl = mons_create (dlevel, zloc, yloc, xloc, &m1);
	ItemID item;
	//int num = rn(40)+20;
	struct Item_internal myitem = new_item (ITYP_GOLD_PIECE);
	it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, 0}});
	myitem = new_item (ITYP_DAGGER);
	item = it_create (&myitem, (union ItemLoc) { .dlvl = {LOC_INV, pl, 1}});
	mons_wield (pl, 0, item);
	//struct Skill skill = {SK_USE_DAGGER, 0, 1};
	//mons_skill_push (pl, &skill); TODO
	myitem = new_item (ITYP_LEATHER_HAT);
	item = it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, 2}});
	mons_wear (pl, item, offsetof (struct WoW, heads[0]));
	myitem = new_item (ITYP_CLOTH_TUNIC);
	item = it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, 3}});
	mons_wear (pl, item, offsetof (struct WoW, torsos[0]));
	myitem = new_item (ITYP_FORCE_SHARD);
	int i;
	for (i = 4; i < 10; ++ i)
		item = it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, i}});
	myitem = new_item (ITYP_FIRE_AXE);
	it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, i}});
	/*int i;
	for (i = 4; i < 40; ++ i)
	{
		myitem = new_item (ityps[ITYP_CLOTH_TUNIC]);
		item = it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, i}});
	}*/
	/*myitem = new_item (ityps[ITYP_GLOVE]);
	it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, 3}});
	myitem = new_item (ityps[ITYP_CHAIN_MAIL]);
	it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, 4}});
	myitem = new_item (ityps[ITYP_HELMET]);
	it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, 5}});
	myitem = new_item (ityps[ITYP_GLOVE]);
	it_create (&myitem, (union ItemLoc) { .inv = {LOC_INV, pl, 6}});*/
	//dlv_addplayer (dlevel, pl);
	//v_push (dlevel->playerIDs, &pl);
	return pl;
}

/* start of and during level */
MonsID gen_mons (int dlevel, int difficulty, int near_player)
{
	int i;
	uint32_t xloc, yloc;
	int zloc;
	for (i = 0; i < 5; ++ i)
	{
		xloc = rn(gen_w), yloc = rn(gen_h), zloc = rn(5)+2;
		//if (near_player && cur_dlevel->player_dist[map_buffer(yloc, xloc)] == -1)
		//	continue;
		if (is_safe_gen (dlevel, zloc, yloc, xloc))
			break;
	}
	if (i >= 5)
		return 0;

	struct Monster_internal p;
	init_mons (&p, mons_gen_type (difficulty));
	if (p.mflags & FL_HOSTILE)
		p.ctr.mode = CTR_AI_HOSTILE;
	else
		p.ctr.mode = CTR_AI_TIMID;
	p.level = 1; //mons[p.type].exp? TODO
	MonsID mons = mons_create (dlevel, zloc, yloc, xloc, &p);
	//printf ("successful generation \n");
	return mons;
}

MonsID gen_mons_in_level (int dlevel, int diff)
{
	return gen_mons (dlevel, diff, 0);
}

MonsID gen_mons_near_player (int dlevel, int diff)
{
	return gen_mons (dlevel, diff, 1);
}

MonsID gen_boss (int dlevel, int zloc, int yloc, int xloc)
{
	struct Monster_internal p;
	init_mons (&p, MTYP_dwarf);
	if (p.mflags & FL_HOSTILE)
		p.ctr.mode = CTR_AI_HOSTILE;
	else
		p.ctr.mode = CTR_AI_TIMID;
	p.level = 1; //mons[p.type].exp? TODO
	MonsID mons = mons_create (dlevel, zloc, yloc, xloc, &p);
	return mons;
}

