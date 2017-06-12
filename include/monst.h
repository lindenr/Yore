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

/* Size flags with _rough_ approximations         */
#define FL_SIZ1 0x20000000      /* tiny (bee)     */
#define FL_SIZ2 0x40000000      /* small (cat)    */
#define FL_SIZ3 0x60000000      /* m-small (dog)  */
#define FL_SIZ4 0x80000000      /* medium (human) */
#define FL_SIZ5 0xA0000000      /* large (horse)  */
#define FL_SIZ6 0xC0000000      /* huge (giant)   */
extern const int CORPSE_WEIGHTS[];

#define FL_BLOB (FL_FLSH | FL_ACID)
#define FL_HMND (FL_FLSH | FL_ARMS | FL_LEGS | FL_TRSO | FL_MIND)
#define FL_SKEL (FL_HMND & (~FL_FLSH))	/* skeletal */

/* Flags for U.m_glflags */
#define MGL_GSAT 0x00000001     /* Satan has been generated */

#define A_PARAM 3				/* AdB damage, of type C */
#define A_NUM   6				/* number of normal attacks per monster (like NH) */
// extern const int CORPSE_WEIGHTS[7];

#define get_filename() "Yore-savegame.sav"

#define WOW_INIT 0

enum MTYPES
{
	MTYP_CHICKEN = 0,
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

struct WoW						/* Wielded or Worn */
{
	struct Item *head, *torso, *legs, *feet, *hands, *arms, *rfin, *lfin; /* armour */
	struct Item *rweap, *lweap;	/* weapon(s) */
	bool two_weaponing;
};

#define SENSE_NONE    0
#define SENSE_VISION  1
#define SENSE_HEARING 2

#define M_EATING 1
#define M_POLY   2

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

//#define BELT_JEWELS 5

/* For U.playing: */
#define PLAYER_STARTING (-1)
#define PLAYER_PLAYING  0
#define PLAYER_LOSTGAME 1
#define PLAYER_WONGAME  2
#define PLAYER_SAVEGAME 3
#define PLAYER_CHEATER  4
#define PLAYER_ERROR    5

typedef uint32_t player_attr[6];

struct player_status
{
	int32_t hunger, oldhunger;
	struct Thing *pl;
	int role;
	int playing;
	player_attr attr; /* st, co, ch, etc */
	int32_t luck;
	uint64_t m_glflags;
	//int magic;
//	enum JEWEL_TYPE jewel[BELT_JEWELS];
};
extern struct player_status U;

enum ABLTY /* ability */
{
	AB_ST = 0,
	AB_CO,
	AB_DX,
	AB_WI,
	AB_IN,
	AB_CH
};

/* The physical method of attack (bite, claw etc) */
enum ATTK_METHOD
{
	ATTK_NONE = 0,				/* placeholder */
	ATTK_HIT,					/* weapon */
	ATTK_TOUCH,					/* for slow mons */
	ATTK_PASS,					/* passive */
	ATTK_MAGIC,					/* purely magical */
	ATTK_BITE,					/* bite */
	ATTK_CLAW					/* scratch */
};

/* The magical method of attack (fire, acid etc -- phys is *not* magical) */
enum ATTK_TYPE
{
	ATYP_PHYS = 0,				/* no magic attack, purely physical (like a
								   soldier with a sword - nothing special) */
	ATYP_COLD,
	ATYP_FIRE,
	ATYP_HEAL,
	ATYP_CURS,
	ATYP_ACID,
	ATYP_BEAM
};

struct monster_struct
{
	const char *name;			/* name */
	char ch;					/* display character */
	uint32_t speed;				/* normal speed */
	uint32_t attacks[A_NUM][A_PARAM];	/* attacks */
	uint32_t flags;				/* physical flags */
	uint32_t col;				/* colour */
	uint32_t exp;				/* exp gained from kill */
};

extern const struct monster_struct mons[];

struct Monster
{
	uint32_t type;				/* monster type */
	int32_t level;				/* EXP level */
	int32_t exp;				/* EXP points */
	int32_t HP;					/* current HP */
	int32_t HP_max;				/* maximum HP */
	uint32_t cur_speed;			/* current speed-state */
	char *name;					/* label */
	struct Pack pack;			/* inventory */
	struct WoW wearing;			/* stuff wielding/wearing/using */
	uint32_t status;			/* is it eating polymorphed etc */
	struct Item *eating;		/* eating something (0 if not) */
};

struct Thing;
/* general monster functions */
void   mons_attack     (struct Thing *, int, int);         /* attack in direction                      */
int    mons_move       (struct Thing *, int, int);         /* move in given directions                 */
void   mons_dead       (struct Thing *, struct Thing *);   /* this monster is dead                     */
int    mons_prhit      (struct Thing *, struct Thing *, int); /* monster hit by a projectile           */
int    mons_take_move  (struct Thing *);                   /* give a move (AI or player)               */
bool   mons_unwield    (struct Thing *);                   /* unwield what is currently wielded        */
bool   mons_wield      (struct Thing *, struct Item *);    /* wield an item (any item)                 */
void   mons_eat        (struct Thing *, struct Item *);    /* eat something                            */
bool   mons_eating     (struct Thing *);                   /* continue eating something                */
bool   mons_can_hear   (struct Thing *);                   /* has ears? no?                            */
void  *mons_get_weap   (struct Thing *);                   /* what weapon is wielded?                  */
void   mons_blast      (struct Thing *, struct Thing *, int); /* monster in an explosion               */

/* player functions */
struct Item *player_use_pack (char *, uint32_t);           /* asks player for an item of some type     */
int    player_gen_type (void);                             /* get a valid monster type for fighting    */
void   player_dead     (const char *, ...);                /* the player is dead; absolute end of game */
void   player_exc      (enum ABLTY, uint32_t);             /* exercise ability by a given amount       */
int    player_sense    (int, int, int);                    /* can the player sense a square            */
ityp   find_corpse     (struct Thing *);                   /* gets a corpse type for a given monster   */
void   custom_free     (void);                             /* frees custom types (now only corpses)    */

/* player_status functions */
char  *get_hungerstr   (void);                             /* gets player's hunger ("Starved" etc)     */
bool   digesting       (void);                             /* is the player digesting?                 */
void   setup_U         (void);                             /* populate the U struct                    */
void   get_cinfo       (void);                             /* called at start, gets input from player  */

void   do_attack       (struct Thing *, struct Thing *);   /* applies an attack                        */
void  *get_sqmons      (Vector *, int, int);               /* returns the monster on a square          */
SqAttr get_sqattr      (Vector *, int, int);               /* returns the SqAttr of a square           */
int    can_amove       (int);                              /* returns if a square can be moved on to   */
char   escape          (unsigned char);                    /* escapes a character                      */

/* AI functions */
int    AI_Attack       (struct Thing *, int, int);         /* moves a monster towards the player       */

#endif /* MONST_H_INCLUDED */
