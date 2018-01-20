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
	MTYP_CHICKEN = 0,
	MTYP_RABBIT,
	MTYP_CRAB,
	MTYP_GNOME,
	MTYP_HUMAN,
	MTYP_DWARF,
	MTYP_HOBBIT,
	MTYP_LICH,
	MTYP_ACID_BLOB,
	MTYP_SATAN,
	NUM_MONS
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
	struct Item *head[MMAX_HEADS], *torso[MMAX_TORSOS],
	            *legs[MMAX_LEGS], *feet[MMAX_LEGS],
				*hands[MMAX_ARMS], *arms[MMAX_ARMS]; /* armour */
	struct Item *weaps[MMAX_ARMS]; /* weapon(s) */
};

#define SENSE_NONE    0
#define SENSE_VISION  1
#define SENSE_HEARING 2

#define M_EATING 1
#define M_POLY   2
#define M_CHARGE 4

enum S_HUN
{
	S_HUN_FULL = 0,
	S_HUN_NOT,
	S_HUN_GRY,
	S_HUN_VERY,
	S_HUN_STA
};

#define HN_LIMIT_1 100
#define HN_LIMIT_2 500
#define HN_LIMIT_3 900
#define HN_LIMIT_4 1400
#define HN_LIMIT_5 2000
extern char *s_hun[];

/* For U.playing: */
#define PLAYER_STARTING (-1)
#define PLAYER_PLAYING  0
#define PLAYER_LOSTGAME 1
#define PLAYER_WONGAME  2
#define PLAYER_SAVEGAME 3
#define PLAYER_CHEATER  4
#define PLAYER_ERROR    5

enum ABLTY /* attribute */
{
	AB_ST = 0,
	AB_CO,
	AB_DX,
	AB_WI,
	AB_IN,
	AB_CH
};

typedef uint32_t player_attr[6];

struct player_status
{
	struct Monster *pl;
	int role;
	int playing;
	player_attr attr; /* st, co, ch, etc */
	int32_t luck;
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

typedef int TID;

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
		TID ID;
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
		TID toID;
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
	TID ID;                     /* thing ID                     */
	int dlevel;                 /* parent dungeon level         */
	int yloc, xloc;             /* location in dungeon          */
	const char *mname;          /* name of monster type         */
	glyph gl;                   /* display glyph                */
	union CTRState ctr;         /* state of the control method  */
	int32_t level;              /* experience level             */
	int32_t exp;                /* experience points            */
	char *name;                 /* monster name                 */

	/* stats which regenerate automatically */
	int32_t HP, HP_max;         /* current/max HP               */
	int32_t ST, ST_max;         /* current/max stamina          */
	//int32_t MP, MP_max;       /* current/max mana TODO        */
	/* fixed stats (may be trained?) */
	uint32_t str;               /* strength (HP regen)          */
	uint32_t con;               /* constitution (ST regen)      */
	//uint32_t wis;             /* wisdom (MP regen) TODO       */
	uint32_t agi;               /* agility (chance to dodge)    */
	uint32_t dex;               /* dexterity (chance to hit)    */
	uint32_t speed;             /* speed (movement delay) */

	struct Pack *pack;          /* inventory                    */
	struct WoW wearing;         /* stuff wielding/wearing/using */
	struct MStatus status;      /* eating, positioning info etc */
	//struct Item *eating;      /* eating something (0 if not)  */
	uint32_t mflags;            /* physical flags               */
	uint8_t boxflags;           /* boxes appearing              */
	Vector skills;              /* available skills             */
};

extern const struct Monster all_mons[];

/* general monster functions */
int    mons_move       (struct Monster *, int, int);         /* move in given directions                 */
int    mons_take_turn  (struct Monster *);                   /* give a move (AI or player)               */
void   mons_box        (struct Monster *, BoxType);          /* boxy flags for this turn                 */
void   mons_usedturn   (struct Monster *);                   /* turn is irretrievably used               */
void   mons_corpse     (struct Monster *, Ityp *);           /* make itype corpse type of the monster    */
Tick   mons_tmgen      ();                                   /* time until next monster generation       */
Tick   mons_tregen     (struct Monster *);                   /* time between regen events                */
int    mons_hits       (struct Monster *, struct Monster *, struct Item *); /* will it hit               */
int    mons_hitdmg     (struct Monster *, struct Monster *, struct Item *); /* how much damage           */
int    mons_ST_hit     (struct Monster *, struct Item *);    /* how much stamina will it consume         */
int    mons_HP_regen   (struct Monster *);                   /* how much HP will regen                   */
int    mons_HP_max_regen(struct Monster *);                  /* similarly for max HP                     */
int    mons_ST_regen   (struct Monster *);                   /* stamina                                  */
int    mons_ST_max_regen(struct Monster *);                  /* max stamina                              */
int    mons_isplayer   (struct Monster *);                   /* is controlled by human                   */
int    mons_cont       (struct Monster *, MCont, union ContData *);/* continuation to be called next turn      */

/* player functions */
struct Item *player_use_pack (struct Monster *, char *, uint32_t); /* ask player for an item             */
int    player_gen_type (void);                               /* get a valid monster type for fighting    */

/* player_status functions */
void   setup_U         (void);                               /* populate the U struct                    */
void   get_cinfo       (void);                               /* called at start, gets input from player  */

int    can_amove       (int);                                /* returns if a square can be moved on to   */
char   escape          (char);                               /* escapes a character                      */

/* AI functions */
int    AI_TIMID_take_turn    (struct Monster *);             /* decide what to do if not attacking       */
int    AI_HOSTILE_take_turn  (struct Monster *);             /* if hostile TODO                          */
int    AI_AGGRO_take_turn    (struct Monster *);             /* if attacking                             */

#endif /* MONST_H_INCLUDED */

