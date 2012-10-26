#if defined(USING_COL)
#  define COL(cl) cl
#else
#  define COL(cl) 0
#endif

#define NUM_MONS ((sizeof(mons)/sizeof(*mons))-1)

#define MONST(nm,ch,sp,at,fl,cl,ex) {nm,ch,sp,at,fl,COL(cl),ex}
#define ATTK(a1,a2,a3,a4,a5,a6) {a1,a2,a3,a4,a5,a6}
#define AM(a,b) ((a)|((b)<<16))
#define A(a,b,c) {a,b,c}
#define AT_NONE {0,0,0}

#define FL_NONE 0x00000000		/* placeholder */
#define FL_UNIQ 0x00000001		/* unique */
#define FL_FLSH 0x00000002		/* fleshy */
#define FL_ACID 0x00000004		/* acidic */
#define FL_ARMS 0x00000010		/* has arms */
#define FL_LEGS 0x00000020		/* has legs */
#define FL_TRSO 0x00000040		/* has torso */
#define FL_MIND 0x00000080		/* mindful */
#define FL_WING 0x00000100		/* has wings */

/* Size flags with _rough_ approximations */
#define FL_SIZ1 0x20000000		/* tiny (bee) */
#define FL_SIZ2 0x40000000		/* small (cat) */
#define FL_SIZ3 0x60000000		/* m-small (dog) */
#define FL_SIZ4 0x80000000		/* medium (human) */
#define FL_SIZ5 0xA0000000		/* large (horse) */
#define FL_SIZ6 0xC0000000		/* huge (giant) */
const int CORPSE_WEIGHTS[7] = { 0, 100, 1000, 3000, 20000, 50000, 300000 };

#define FL_BLOB (FL_FLSH | FL_ACID)
#define FL_HMND (FL_FLSH | FL_ARMS | FL_LEGS | FL_TRSO | FL_MIND)	/* humanoid 
																	 */
#define FL_SKEL (FL_HMND & (~FL_FLSH))	/* skeletal */

/* Flags for U.m_glflags */
#define MGL_GSAT 0x00000001     /* Satan has been generated */

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
		  4),
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
