#ifndef MONST_H_INCLUDED
#define MONST_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"
#include "include/pack.h"

enum MONS_BODYPART
{
	MONS_HAND = 0,
	MONS_TORSO,
	MONS_HEAD,
	MONS_FOOT
};

#define FL_NONE 0x00000000      /* placeholder    */
#define FL_UNIQ 0x00000001      /* unique         */
#define FL_FLSH 0x00000002      /* fleshy         */
#define FL_ACID 0x00000004      /* acidic         */
#define FL_ARMS 0x00000010      /* has arms       */
#define FL_LEGS 0x00000020      /* has legs       */
#define FL_TRSO 0x00000040      /* has torso      */
#define FL_MIND 0x00000080      /* mindful        */
#define FL_WING 0x00000100      /* has wings      */
#define FL_SLIMY 0x00000200     /* slimy          */

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
struct Skill;
typedef struct Skill *Skill;

enum MTYPE
{
	MTYP_chicken = 0,
	MTYP_newt,
	MTYP_slime_rat,
	MTYP_spider,
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
	ItemID heads[MMAX_HEADS], torsos[MMAX_TORSOS],
		legs[MMAX_LEGS], feet[MMAX_LEGS],
		hands[MMAX_ARMS]; /* armour */
	ItemID weaps[MMAX_ARMS]; /* weapon(s) */
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

/* type of damage inflicted */
enum DMG_TYPE
{
	DTYP_CUT = 0,
	DTYP_BLUNT,
//	DTYP_HEAL,
//	DTYP_ACID
	DTYP_COLD,
	DTYP_FIRE,
	DTYP_BLEED
};

typedef int MonsID;

typedef enum
{
	CTR_NONE = 0,   /* placeholder */
	CTR_PL,         /* player control (standard) */
#ifdef SIM
	CTR_AI_SIM_FARMER,
#endif /* SIM */
	CTR_AI_TIMID,   /* neutral non-player monster */
	CTR_AI_HOSTILE, /* will anger on sight */
	CTR_AI_AGGRO    /* angry non-player monster */
} CTR_MODE;

struct Monster;
union CTRState
{
	CTR_MODE mode;
	struct
	{
		CTR_MODE mode;
	} timid;
	struct
	{
		CTR_MODE mode;
		MonsID ID;
	} aggro;
};

struct MStatus
{
#include "auto/monst.status.h"
};

#define DEF_MSTATUS ((struct MStatus){{0,},})

struct Monster_internal
{
	MonsID ID;                /* monster ID                   */
	int dlevel;            /* parent dungeon level         */
	int zloc, yloc, xloc;  /* location in dungeon          */
	enum MTYPE mtype;      /* monster type                 */
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

	struct Pack pack;      /* inventory                    */
	struct WoW wearing;    /* stuff wielding/wearing/using */
	struct MStatus status; /* eating, positioning info etc */
	uint32_t mflags;       /* physical flags               */
	Vector skills;         /* available skills             */
};

extern const struct Monster_internal mons_types[];

int    mons_is         (MonsID);             /* does the monster exist                   */
void   mons_poll       (MonsID);             /* poll for action (AI or player)           */
glyph  mons_gl         (MonsID);

struct Monster_internal *mons_internal (MonsID);

/* monster action functions */

/* move */
int    mons_can_move   (MonsID, int, int, int);    /* can mons move in given direction   */
void   mons_try_move   (MonsID, int, int, int);    /* move in given directions           */
void   mons_start_move (MonsID, int, int, int, Tick);/* start moving in a given direction*/
void   mons_stop_move  (MonsID);             /* stop moving                              */

/* hit */
void   mons_try_hitm   (MonsID, int, int);   /* attack a given monster                   */
void   mons_try_hit    (MonsID, int, int);   /* attack in given direction                */
void   mons_start_hit  (MonsID, int, int,    /* start attacking                          */
	int, Tick);
void   mons_stop_hit   (MonsID);             /* stop attacking                           */

/* evade */
void   mons_try_evade  (MonsID, int, int);   /* evade in given direction                 */
void   mons_start_evade(MonsID, int, int,    /* start evading in a given direction       */
	Tick, Tick);
void   mons_stop_evade (MonsID);             /* stop evading                             */

/* wear/take off */
int    mons_can_wear   (MonsID, ItemID,      /* can it be worn                           */
	size_t);
int    mons_try_wear   (MonsID, ItemID);     /* wear something                           */
void   mons_wear       (MonsID, ItemID,      /* start wearing an item                    */
	size_t);
void   mons_take_off   (MonsID, ItemID);     /* take off an item                         */
int    mons_can_takeoff(MonsID, ItemID);     /* can it be taken off                      */
int    mons_try_takeoff(MonsID, ItemID);     /* take off some armour                     */

/* wield/unwield */
void   mons_try_wield  (MonsID, ItemID);     /* wield an item                            */
void   mons_wield      (MonsID, int, ItemID);/* wield an item in an arm                  */
void   mons_unwield    (MonsID, int);        /* unwield an item from a given arm         */

/* charging */
int    mons_charging   (MonsID);

/* misc */
void   mons_corpse     (MonsID,              /* make itype corpse type of the monster    */
	struct Item_internal *);
Tick   mons_tregen     (MonsID);             /* time between regen events                */
int    mons_throwspeed (MonsID, ItemID);     /* how fast the item can be thrown          */
int    proj_hitm       (ItemID, MonsID);     /* will the projectile hit                  */
int    proj_hitdmg     (ItemID, MonsID);     /* how much damage                          */
int    mons_hitm       (MonsID, MonsID,      /* will the monster hit                     */
	ItemID);
int    mons_hitdmg     (MonsID, MonsID,      /* how much damage                          */
	ItemID);
int    mons_ST_hit     (MonsID, ItemID);     /* how much stamina will it consume         */
int    mons_HP_regen   (MonsID);             /* how much HP will regen                   */
int    mons_ST_regen   (MonsID);             /* stamina                                  */
int    mons_MP_regen   (MonsID);             /* MP                                       */
int    mons_isplayer   (MonsID);             /* is controlled by human                   */
const char *mons_typename (MonsID);          /* get name of monster type                 */
int    mons_get_HP     (MonsID);             /* recalculates max HP                      */
int    mons_get_ST     (MonsID);             /* max stamina                              */
int    mons_get_MP     (MonsID);             /* max magic power                          */
int    mons_speed      (MonsID);
int    mons_gets_exp   (MonsID);             /* does the monster level up                */
int    mons_get_level  (MonsID);
void   mons_get_exp    (MonsID, int);        /* gain exp                                 */
int    mons_level      (int exp);            /* what level a given experience is         */
void   mons_getloc     (MonsID, int *, int *, int *, int *);
int    mons_exp_needed (int level);          /* exp needed for next level                */
int    mons_skill      (MonsID, ItemID);     /* get skill level for using an item to hit */
Vector mons_skills     (MonsID);
int    mons_exp        (MonsID);
int    mons_str        (MonsID);
int    mons_con        (MonsID);
int    mons_wis        (MonsID);
int    mons_armour     (MonsID);
CTR_MODE mons_ctrl     (MonsID);
int    mons_attk_bonus (MonsID, ItemID);     /* get extra damage a monster does          */
enum MTYPE mons_type   (MonsID);             /* get monster type                         */
int    mons_can_bleed  (MonsID);             /* can it bleed                             */
int    mons_bleeding   (MonsID);
int    mons_index      (MonsID);             /* dlevel index of the monster              */
int    mons_dlevel     (MonsID);
struct DLevel *mons_dlv(MonsID);
struct Pack *mons_pack (MonsID);
void   mons_setweap    (MonsID, int, ItemID);
ItemID mons_getweap    (MonsID, int arm);    /* get weapon wielded in arm                */

/* other effects */
void   mons_tilefrost  (MonsID, int, int, int);/* induce a frost effect                   */
int    mons_take_damage(MonsID, MonsID,       /* returns whether to-monster still alive   */
	int, enum DMG_TYPE);
void   mons_kill       (MonsID, MonsID);      /* kill a given monster                     */
void   mons_dead       (MonsID);              /* monster is dead - add corpse etc         */
void   mons_anger      (MonsID, MonsID);      /* monster angers another monster           */
void   mons_calm       (MonsID);              /* monster calms                            */
void   mons_stats_changed (MonsID);           /* update HP etc to reflect stats           */
void   mons_exercise   (MonsID, ItemID);      /* exercise a weapon use                    */
void   mons_ex_skill   (MonsID, Skill);       /* exercise a skill                         */
void   mons_startbleed (MonsID);              /* start bleeding                           */

/* player functions */
int    mons_gen_type   (void);                               /* get a valid monster type for fighting    */

/* AI functions */
void   AI_TIMID_poll   (MonsID);                   /* decide what to do if not attacking       */
void   AI_HOSTILE_poll (MonsID);                   /* if hostile TODO                          */
void   AI_AGGRO_poll   (MonsID);                   /* if attacking                             */

#endif /* MONST_H_INCLUDED */

