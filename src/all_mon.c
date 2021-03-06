/* all_mon.c */

#include "include/all.h"
//#include "include/thing.h"
//#include "include/panel.h"
#include "include/monst.h"

#define MONST(nm,ch,size,at,fl,cl,str,con,wis,agi,spd,ex) \
{0,0,-1,-1,-1,-1,nm,ch|cl,{.mode=CTR_NONE},0,ex,0,NULL, \
1, 1, 1, 1, 1, 1, str, con, wis, agi, spd, {{0,}}, {1, 1, 2, 2, {0,},{0,},{0,},{0,},{0,},{0,}},\
DEF_MSTATUS,(fl)|(FL_SIZE(size)),NULL}

#define ATTK(a1,a2,a3,a4,a5,a6) {a1,a2,a3,a4,a5,a6}
#define AM(a,b) ((a)|((b)<<16))
#define A(a,b,c) {a,b,c}
#define AT_NONE {0,0,0}
//#define STAT(con,str,agi,dex,speed) {con,con,str*4,str*4,con,str,agi,dex,speed}
#define SIZE(n) n

const int CORPSE_WEIGHTS[7] = {0, 100, 1000, 3000, 20000, 50000, 300000};

const struct Monster_internal mons_types[] = {
	MONST("chicken", 'c', SIZE(2),
		  ATTK(A(1, 2, ATTK_BITE), A(2, 3, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_NEUTRAL | FL_FLSH | FL_WING,
		  COL_TXT_RED(15) | COL_TXT_GREEN(15),
		  8, 0, 0, 3, 1000, 3),
	MONST("newt", ':', SIZE(1),
		  ATTK(A(1, 2, ATTK_BITE), A(2, 3, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_FLSH,
		  COL_TXT(15, 15, 0),
		  8, 0, 2, 3, 1000, 3),
	MONST("slime rat", 'r', SIZE(1),
		  ATTK(A(1, 2, ATTK_BITE), A(2, 3, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_FLSH | FL_SLIMY,
		  COL_TXT(7, 8, 0),
		  8, 0, 2, 3, 1000, 3),
	MONST("spider", 0x0F, SIZE(1),
		  ATTK(A(1, 2, ATTK_BITE), A(2, 3, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_FLSH,
		  COL_TXT(0,15,0),
		  8, 0, 2, 3, 747, 3),
	MONST("skeleton", 's', SIZE(4),
		  ATTK(A(1, 2, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_SKEL,
		  COL_TXT(15, 15, 15),
		  20, 5, 2, 3, 1000, 100),
	MONST("rabbit", 'g', SIZE(2),
		  ATTK(A(1, 2, ATTK_BITE), A(2, 3, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_NEUTRAL | FL_FLSH,
		  COL_TXT_RED(11) | COL_TXT_GREEN(15),
		  2, 1, 0, 3, 400, 4),
	MONST("crab", 'c', SIZE(2),
		  ATTK(A(2, 5, ATTK_CLAW), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_FLSH | FL_ARMS | FL_LEGS,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  2, 2, 0, 3, 1000, 5),
	MONST("gnome", 'G', SIZE(3),
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_HMND,
		  COL_TXT_RED(11) | COL_TXT_GREEN(11),
		  5, 5, 1, 2, 800, 8),
	MONST("human", '@', SIZE(4),
		  ATTK(A(1, 4, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_HMND,
		  COL_TXT(15, 15, 15),
		  10, 10, 5, 10, 1000, 200),
	MONST("dwarf", 'h', SIZE(3),
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HMND,
		  COL_TXT_RED(11),
		  15, 12, 5, 5, 1000, 250),
	MONST("hobbit", 'h', SIZE(3),
		  ATTK(A(1, 3, ATTK_HIT), AT_NONE, AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_HMND,
		  COL_TXT_GREEN(11),
		  8, 8, 5, 12, 1000, 100),
	MONST("lich", 'L', SIZE(4),
		  ATTK(A(2, 4, ATTK_HIT), A(2, 2, ATTK_TOUCH), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_SKEL,
		  COL_TXT_RED(11) | COL_TXT_BLUE(11),
		  200, 200, 200, 200, 600, 10000)/*,
	MONST("acid blob", 'j', 2000, SIZE(3), 5,
		  ATTK(A(2, 4, ATTK_TOUCH), A(3, 2, AM(ATTK_PASS, ATYP_ACID)), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_BLOB,
		  COL_TXT_GREEN(11),
		  30, 10, 3)*/,
	MONST("Satan", 'D', SIZE(5),
		  ATTK(A(3, 5, ATTK_HIT), A(4, 5, ATTK_CLAW), AT_NONE,
			   AT_NONE, AT_NONE, AT_NONE), FL_HOSTILE | FL_UNIQ, COL_TXT_RED(11),
		  300, 300, 300, 300, 1200, 12000),
	MONST(NULL, 0, SIZE(0),
		  ATTK(AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE), 0, 0, 0, 0, 0, 0, 0, 0)
};

