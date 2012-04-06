#if defined(USING_COL)
#  define COL(cl) cl
#else
#  define COL(cl)
#endif

#define MONST(nm,ch,sp,at,fl,cl,ex) {nm,ch,sp,at,fl,COL(cl),ex}
#define ATTK(a1,a2,a3,a4,a5,a6) {a1,a2,a3,a4,a5,a6}
#define AM(a,b) ((a)|((b)<<16))
#define A(a,b,c) {a,b,c}
#define AT_NONE {0,0,0}

#define FL_NONE 0x00000000 /* placeholder */
#define FL_UNIQ 0x00000001 /* unique      */
#define FL_FLSH 0x00000002 /* fleshy      */
#define FL_ACID 0x00000004 /* acidic      */
#define FL_ARMS 0x00000010 /* has arms    */
#define FL_LEGS 0x00000020 /* has legs    */
#define FL_TRSO 0x00000040 /* has torso   */
#define FL_MIND 0x00000080 /* mindful     */
#define FL_BLOB (FL_FLSH | FL_ACID)
#define FL_HMND (FL_FLSH | FL_ARMS | FL_LEGS | FL_TRSO | FL_MIND) /* humanoid */
#define FL_SKEL (FL_HMND & (~FL_FLSH)) /* skeletal */

const struct monster_struct mons[] = {
MONST("gnome", 'G', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), FL_HMND, COL_TXT_RED|COL_TXT_GREEN,
      36),
MONST("human", '@', 12,
      ATTK(A(1,4,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), FL_HMND, COL_TXT_BLUE|COL_TXT_RED|COL_TXT_GREEN,
      48),
MONST("dwarf", 'h', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), FL_HMND, COL_TXT_RED,
      36),
MONST("hobbit", 'h', 10,
      ATTK(A(1,3,ATTK_HIT), AT_NONE, AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), FL_HMND, COL_TXT_GREEN,
      45),
MONST("lich", 'L', 8,
      ATTK(A(2,4,ATTK_HIT), A(2,2,AM(ATTK_MAGIC,ATYP_CURS)), AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), FL_SKEL, COL_TXT_RED|COL_TXT_BLUE,
      216),
MONST("acid blob", 'j', 6,
      ATTK(A(2,4,ATTK_TOUCH), A(3,2,AM(ATTK_PASS,ATYP_ACID)), AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), FL_BLOB, COL_TXT_GREEN,
      30),
MONST("Satan", 'j', 6,
      ATTK(A(2,4,ATTK_TOUCH), A(3,2,AM(ATTK_PASS,ATYP_ACID)), AT_NONE,
           AT_NONE, AT_NONE, AT_NONE), FL_UNIQ, COL_TXT_GREEN,
      1024)
};

