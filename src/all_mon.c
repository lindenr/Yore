/* all_mon.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/panel.h"
#include "include/rand.h"
#include "include/monst.h"

#if defined(USING_COL)
#  define COL(cl) cl
#else
#  define COL(cl) 0
#endif

#define MONST(nm,ch,sp,at,fl,cl,ex,hp,st) \
{nm,ch|COL(cl),{.mode=AI_NONE},0,ex,NULL, \
hp, hp, st, st, 0, 0, 0, 0, sp, NULL, \
{{0,0},{0,0,0},0},fl,0,NULL}

#define ATTK(a1,a2,a3,a4,a5,a6) {a1,a2,a3,a4,a5,a6}
#define AM(a,b) ((a)|((b)<<16))
#define A(a,b,c) {a,b,c}
#define AT_NONE {0,0,0}
#define STAT(con,str,agi,dex,speed) {con,con,str,str,con,str,agi,dex,speed}

const int CORPSE_WEIGHTS[7] = {0, 100, 1000, 3000, 20000, 50000, 300000};

const struct Monster all_mons[] = {
	MONST("chicken", 'c', 1000,
		  ATTK(A(1, 2, ATTK_BITE), A(2, 3, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_FLSH | FL_WING | FL_SIZ2,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11) | COL_TXT_BRIGHT,
		  6, 5, 5),
	MONST("rabbit", 'g', 400,
		  ATTK(A(1, 2, ATTK_BITE), A(2, 3, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_FLSH | FL_SIZ2,
		  COL_TXT_RED(11) | COL_TXT_GREEN(15),
		  6, 4, 8),
	MONST("crab", 'c', 1000,
		  ATTK(A(2, 5, ATTK_CLAW), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_FLSH | FL_ARMS | FL_LEGS,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  1, 7, 3),
	MONST("gnome", 'G', 1000,
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ3,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  36, 15, 10),
	MONST("human", '@', 1000,
		  ATTK(A(1, 4, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ4,
		  COL_TXT_BLUE(11) | COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  48, 20, 15),
	MONST("dwarf", 'h', 1000,
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ3, COL_TXT_RED(11),
		  36, 25, 10),
	MONST("hobbit", 'h', 1000,
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ3, COL_TXT_GREEN(11),
		  45, 20, 15),
	MONST("lich", 'L', 1200,
		  ATTK(A(2, 4, ATTK_HIT), A(2, 2, AM(ATTK_MAGIC, ATYP_CURS)), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_SKEL | FL_SIZ4,
		  COL_TXT_RED(11) | COL_TXT_BLUE(11),
		  216, 45, 25),
	MONST("acid blob", 'j', 2000,
		  ATTK(A(2, 4, ATTK_TOUCH), A(3, 2, AM(ATTK_PASS, ATYP_ACID)), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_BLOB | FL_SIZ2, COL_TXT_GREEN(11),
		  30, 10, 3),
	MONST("Satan", 'D', 2000,
		  ATTK(A(3, 5, ATTK_HIT), A(4, 5, AM(ATTK_MAGIC, ATYP_BEAM)), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_UNIQ | FL_SIZ5, COL_TXT_RED(11),
		  1024, 99, 99),
	MONST(NULL, 0, 0,
		  ATTK(AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE), 0, 0, 0, 0, 0)
};

