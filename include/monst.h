#ifndef MONST_H_INCLUDED
#define MONST_H_INCLUDED

#include "include/graphics.h"
#include "include/pack.h"
#include "include/map.h"
#include "include/vector.h"

#define FL_NONE 0x00000000      /* placeholder    */
#define FL_UNIQ 0x00000001      /* unique         */
#define FL_FLSH 0x00000002      /* fleshy         */
#define FL_ACID 0x00000004      /* acidic         */
#define FL_ARMS 0x00000010      /* has arms       */
#define FL_LEGS 0x00000020      /* has legs       */
#define FL_TRSO 0x00000040      /* has torso      */
#define FL_MIND 0x00000080      /* mindful        */
#define FL_WING 0x00000100      /* has wings      */

#define FL_HOSTILE 0x01000000   /* starts hostile */
#define FL_NEUTRAL 0x00000000   /* starts neutral */

/* Size flags with _rough_ approximations         */
#define FL_SIZ0 0x00000000      /* placeholder    */
#define FL_SIZ1 0x20000000      /* tiny (bee)     */
#define FL_SIZ2 0x40000000      /* small (cat)    */
#define FL_SIZ3 0x60000000      /* m-small (dog)  */
#define FL_SIZ4 0x80000000      /* medium (human) */
#define FL_SIZ5 0xA0000000      /* large (horse)  */
#define FL_SIZ6 0xC0000000      /* huge (giant)   */
#define FL_SIZE(n) (n << 29)
extern const int CORPSE_WEIGHTS[];

#define FL_BLOB (FL_FLSH | FL_ACID)
#define FL_HMND (FL_FLSH | FL_ARMS | FL_LEGS | FL_TRSO | FL_MIND)
#define FL_SKEL (FL_HMND & (~FL_FLSH))	/* skeletal */

/* Flags for U.m_glflags */
#define MGL_GSAT 0x00000001     /* Satan has been generated */

#define A_PARAM 3				/* AdB damage, of type C */
#define A_NUM   6				/* number of normal attacks per monster (like NH) */

#define get_filename() "Yore-savegame.sav"

typedef long long unsigned Tick;

enum MTYPES
{
	MTYP_chicken = 0,
	MTYP_newt,
	MTYP_slime_rat,
	MTYP_skeleton,
	MTYP_rabbit,
	MTYP_crab,
	MTYP_gnome,
	MTYP_human,
	MTYP_dwarf,
	MTYP_hobbit,
	MTYP_lich,
	//MTYP_acid_blob,
	MTYP_Satan,
	MTYP_NUM_MONS
};

#define MMAX_HEADS  2
#define MMAX_ARMS   4
#define MMAX_TORSOS 1
#define MMAX_LEGS   8
#define MMAX_WINGS  4
#define MMAX_EYES   3
#define MMAX_FINGS  2 /* one on each hand */

struct WoW /* Wielded or Worn */
{
	int nheads, ntorsos, nlegs, narms;
	struct Item *heads[MMAX_HEADS], *torsos[MMAX_TORSOS],
	            *legs[MMAX_LEGS], *feet[MMAX_LEGS],
				*hands[MMAX_ARMS]; /* armour */
	struct Item *weaps[MMAX_ARMS]; /* weapon(s) */
};

/* For U.playing: */
#define PLAYER_STARTING (-1)
#define PLAYER_PLAYING  0
#define PLAYER_LOSTGAME 1
#define PLAYER_WONGAME  2
#define PLAYER_SAVEGAME 3
#define PLAYER_CHEATER  4
#define PLAYER_ERROR    5

struct player_status
{
	int playing;
	uint64_t m_glflags;
};
extern struct player_status U;

/* The physical method of attack (bite, claw etc) */
enum ATTK_METHOD
{
	ATTK_NONE = 0, /* placeholder */
	ATTK_HIT,      /* weapon */
	ATTK_TOUCH,    /* for slow mons */
	ATTK_PASS,     /* passive */
	ATTK_MAGIC,    /* purely magical */
	ATTK_BITE,     /* bite */
	ATTK_CLAW      /* scratch */
};

/* The magical method of attack (fire, acid etc -- phys is *not* magical) */
enum ATTK_TYPE
{
	ATYP_PHYS = 0,
	ATYP_COLD,
	ATYP_FIRE,
	ATYP_HEAL,
	ATYP_CURS,
	ATYP_ACID,
	ATYP_BEAM
};

typedef int MID;

typedef enum
{
	CTR_NONE = 0,   /* placeholder */
	CTR_PL,         /* player control (standard) */
	CTR_PL_CONT,    /* player control (continuation) */
	CTR_PL_FOCUS,   /* player control (focus mode) */
	CTR_AI_TIMID,   /* neutral non-player monster */
	CTR_AI_HOSTILE, /* will anger on sight */
	CTR_AI_AGGRO    /* angry non-player monster */
} CTR_MODE;

union ContData
{
	struct Item *item;
	Vector vec;
};

struct Monster;
typedef int (*MCont) (struct Monster *);
union CTRState
{
	CTR_MODE mode;
	struct
	{
		CTR_MODE mode;
		MCont cont;
		union ContData data;
	} cont;
	struct
	{
		CTR_MODE mode;
	} timid;
	struct
	{
		CTR_MODE mode;
		MID ID;
	} aggro;
};

struct MStatus
{
	struct
	{
		int ydir, xdir;
	} moving;
	struct
	{
		int ydir, xdir;
		MID toID;
		int arm;
	} attacking;
	int evading;
	struct
	{
		int ydir, xdir;
	} defending;
	int charging;
};

struct Monster
{
	MID ID;                /* monster ID                   */
	int dlevel;            /* parent dungeon level         */
	int yloc, xloc;        /* location in dungeon          */
	enum MTYPES type;      /* type of monster              */
	const char *mname;     /* name of monster type         */
	glyph gl;              /* display glyph                */
	union CTRState ctr;    /* state of the control method  */
	int level;             /* experience level             */
	int exp;               /* experience points            */
	int armour;            /* aids damage reduction        */
	char *name;            /* monster name                 */

	/* stats which regenerate automatically */
	int HP, HP_max;        /* current/max HP               */
	int ST, ST_max;        /* current/max stamina          */
	int MP, MP_max;        /* current/max mana             */
	/* fixed stats (may be trained?) */
	int str;               /* strength (HP max/regen)      */
	int con;               /* constitution (ST max/regen)  */
	int wis;               /* wisdom (MP max/regen)        */
	int agi;               /* agility (affects speed)      */
	int speed;             /* speed (movement delay)       */

	struct Pack *pack;     /* inventory                    */
	struct WoW wearing;    /* stuff wielding/wearing/using */
	struct MStatus status; /* eating, positioning info etc */
	uint32_t mflags;       /* physical flags               */
	Vector skills;         /* available skills             */
};

extern const struct Monster all_mons[];

/* general monster functions */
int    mons_move       (struct Monster *, int, int);         /* move in given directions                 */
int    mons_try_attack (struct Monster *, int, int);         /* attack in given directions               */
int    mons_try_wear   (struct Monster *, struct Item *);    /* wear some armour                         */
int    mons_can_wear   (struct Monster *, struct Item *, size_t); /* can it be worn                      */
int    mons_try_takeoff(struct Monster *, struct Item *);    /* wear some armour                         */
int    mons_can_takeoff(struct Monster *, struct Item *);    /* can it be taken off                      */
int    mons_take_turn  (struct Monster *);                   /* give a move (AI or player)               */
void   mons_corpse     (struct Monster *, struct Item *);    /* make itype corpse type of the monster    */
Tick   mons_tregen     (struct Monster *);                   /* time between regen events                */
int    mons_throwspeed (struct Monster *, struct Item *);    /* how fast the item can be thrown          */
int    proj_hitm       (struct Item *, struct Monster *);    /* will the projectile hit                  */
int    proj_hitdmg     (struct Item *, struct Monster *);    /* how much damage                          */
int    mons_hitm       (struct Monster *, struct Monster *, struct Item *); /* will it hit               */
int    mons_hitdmg     (struct Monster *, struct Monster *, struct Item *); /* how much damage           */
int    mons_ST_hit     (struct Monster *, struct Item *);    /* how much stamina will it consume         */
int    mons_HP_regen   (struct Monster *);                   /* how much HP will regen                   */
int    mons_ST_regen   (struct Monster *);                   /* stamina                                  */
int    mons_MP_regen   (struct Monster *);                   /* MP                                       */
int    mons_isplayer   (struct Monster *);                   /* is controlled by human                   */
int    mons_cont       (struct Monster *, MCont, union ContData *);/* continuation to be called next turn*/
int    mons_get_HP     (struct Monster *);                   /* recalculates max HP                      */
int    mons_get_ST     (struct Monster *);                   /* max stamina                              */
int    mons_get_MP     (struct Monster *);                   /* max magic power                          */
int    mons_gets_exp   (struct Monster *);                   /* does the monster level up                */
int    mons_level      (int exp);                            /* what level a given experience is         */
int    mons_exp_needed (int level);                          /* exp needed for next level                */
void   mons_level_up   (struct Monster *);                   /* alter monster's base stats for new level */
void   mons_stats_changed(struct Monster *);                 /* update HP etc to reflect stats           */
void   mons_exercise   (struct Monster *, struct Item *);    /* exercise a weapon use                    */

/* effects */
void   mons_tilefrost  (struct Monster *, int, int);         /* induce a frost effect                    */
void   mons_wield      (struct Monster *, int, struct Item *); /* wield an item in an arm                */
void   mons_unwield    (struct Monster *, struct Item *);    /* unwield an item */

/* player functions */
int    mons_gen_type   (void);                               /* get a valid monster type for fighting    */

/* map function? */
int    can_amove       (int);                                /* returns if a square can be moved on to   */

/* AI functions */
int    AI_TIMID_take_turn    (struct Monster *);             /* decide what to do if not attacking       */
int    AI_HOSTILE_take_turn  (struct Monster *);             /* if hostile TODO                          */
int    AI_AGGRO_take_turn    (struct Monster *);             /* if attacking                             */

#endif /* MONST_H_INCLUDED */

