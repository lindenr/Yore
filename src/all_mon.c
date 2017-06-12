/* all_mon.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/panel.h"
#include "include/rand.h"
#include "include/loop.h"
#include "include/monst.h"

#if defined(USING_COL)
#  define COL(cl) cl
#else
#  define COL(cl) 0
#endif

#define MONST(nm,ch,sp,at,fl,cl,ex) {nm,ch,sp,at,fl,COL(cl),ex}
#define ATTK(a1,a2,a3,a4,a5,a6) {a1,a2,a3,a4,a5,a6}
#define AM(a,b) ((a)|((b)<<16))
#define A(a,b,c) {a,b,c}
#define AT_NONE {0,0,0}

const int CORPSE_WEIGHTS[7] = {0, 100, 1000, 3000, 20000, 50000, 300000};

const struct monster_struct mons[] = {
	MONST("chicken", 'c', 12,
		  ATTK(A(1, 1, ATTK_BITE), A(1, 1, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_FLSH | FL_WING | FL_SIZ2,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11) | COL_TXT_BRIGHT,
		  6),
	MONST("crab", 'c', 12,
		  ATTK(A(1, 2, ATTK_CLAW), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_FLSH | FL_ARMS | FL_LEGS,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  0),
	MONST("gnome", 'G', 10,
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ3,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  36),
	MONST("human", '@', 12,
		  ATTK(A(1, 4, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ4,
		  COL_TXT_BLUE(11) | COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  48),
	MONST("dwarf", 'h', 10,
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ3, COL_TXT_RED(11),
		  36),
	MONST("hobbit", 'h', 10,
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND | FL_SIZ3, COL_TXT_GREEN(11),
		  45),
	MONST("lich", 'L', 8,
		  ATTK(A(2, 4, ATTK_HIT), A(2, 2, AM(ATTK_MAGIC, ATYP_CURS)), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_SKEL | FL_SIZ4,
		  COL_TXT_RED(11) | COL_TXT_BLUE(11),
		  216),
	MONST("acid blob", 'j', 6,
		  ATTK(A(2, 4, ATTK_TOUCH), A(3, 2, AM(ATTK_PASS, ATYP_ACID)), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_BLOB | FL_SIZ2, COL_TXT_GREEN(11),
		  30),
	MONST("Satan", 'D', 6,
		  ATTK(A(3, 5, ATTK_HIT), A(4, 5, AM(ATTK_MAGIC, ATYP_BEAM)), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_UNIQ | FL_SIZ5, COL_TXT_RED(11),
		  1024),
	MONST(NULL, 0, 0,
		  ATTK(AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE), 0, 0, 0)
};

