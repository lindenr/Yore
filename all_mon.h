#if defined(USING_COL)
#  define COL(cl) cl
#else
#  define COL(cl)
#endif

#define MONST(nm,ch,sp,at,cl) {nm,ch,sp,at,COL(cl)}
#define ATTK(a1,a2,a3,a4,a5,a6) {a1,a2,a3,a4,a5,a6}
#define AM(a,b) ((a)|((b)<<16))
#define A(a,b,c) {a,b,c}
#define AT_NONE {0,0,0}

const struct monster_struct mons[] = {
MONST("gnome", 'G', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_RED|COL_TXT_GREEN),
MONST("human", '@', 12,
      ATTK(A(1,4,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_BLUE|COL_TXT_RED|COL_TXT_GREEN),
MONST("dwarf", 'h', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_RED),
MONST("hobbit", 'h', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_GREEN),
MONST("lich", 'L', 8,
      ATTK(A(2,4,ATTK_HIT), A(2,2,AM(ATTK_MAGIC,ATYP_CURS)), AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_RED|COL_TXT_BLUE),
MONST("acid blob", 'j', 6,
      ATTK(A(2,4,ATTK_TOUCH), A(3,2,AM(ATTK_PASS,ATYP_ACID)), AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), COL_TXT_GREEN)
};
