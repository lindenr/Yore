#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include "include/all.h"

#define NUM_TABS 3

#define PANE_H 10
#define PANE_PH (PANE_H * GLH)

#define PANEL_CLOSED  0
#define PANEL_SELF    1
#define PANEL_MSG     2
#define PANEL_OPTIONS 3

#define P_MSG_LEN 45

struct P_msg
{
	glyph msg[P_MSG_LEN];
};

void p_init    ();
void p_pane    (MonsID);
void p_amsg    (const char *);
void p_msg     (const char *, ...);
char p_ask     (MonsID, const char *, const char *);
char p_lines   (Vector);
void p_anotify (const char *);
void p_notify  (const char *, ...);
void p_endnotify  ();

enum PanelType
{
	P_STATUS = 0,
	P_SKILLS
};

enum P_MV
{
	P_MV_START = 0,
	P_MV_CHOOSING,
	P_MV_END
};

struct MenuOption
{
	int num;
	int len;
	glyph *ex_str;
};

enum P_FORMAT
{
	FMT_LEFT = 0,
	FMT_CENTRE,
	FMT_RIGHT,
	FMT_MENUOPTION
};

struct FormattedGlyph
{
	glyph gl;
	enum P_FORMAT fmt;
};

char *gl_format (glyph gl);
int  p_menuex   (Vector lines, const char *toquit, int max_line_len);
Vector p_formatted(const char *input, int max_line_len);
int  p_flines   (const char *);
void p_ffree    (Vector);
int  p_status   (MonsID, enum PanelType);
int  p_skills   (MonsID, enum PanelType);
int  p_mvchoose (MonsID, int *, int *, int *, const char *, const char *,
	void (*) (enum P_MV, int, int, int, int, int, int, int));

void show_path_on_overlay (enum P_MV, int dlevel, int z, int y, int x, int, int, int);
void show_disc_on_overlay (enum P_MV, int, int, int, int, int, int, int);

char p_getch (MonsID player);

void p_msgbox (const char *msg);

extern int p_width, p_height
//, p_open
;

extern Graph gpan;

#define FCOL(col,str) "#n" col str "#nBBB00000"
#define COL_BRIGHT(str) FCOL("FFF00000", str)
#define COL_RED(str) FCOL("F3300000", str)
#define COL_YELLOW(str) FCOL("FF000000", str)
#define COL_GREEN(str) FCOL("0F000000", str)

struct Item;
struct Skill;
typedef struct Skill *Skill;

/* effects the player might observe */
void eff_mons_fail_throw (MonsID mons, ItemID item);
void eff_item_dissipates (ItemID item);
void eff_item_absorbed (ItemID item);
void eff_item_hits_wall (ItemID item);
void eff_proj_misses_mons (ItemID item, MonsID mons);
void eff_proj_hits_mons (ItemID item, MonsID mons, int damage);
void eff_mons_starts_hit (MonsID mons, int y, int x);
void eff_mons_tiredly_misses_mons (MonsID fr, MonsID to);
void eff_mons_misses_mons (MonsID fr, MonsID to);
void eff_mons_just_misses_mons (MonsID fr, MonsID to);
void eff_mons_hits_mons (MonsID fr, MonsID to, int damage);
void eff_mons_bleeds (MonsID mons, int damage);
void eff_mons_burns (MonsID mons, int damage);
void eff_mons_kills_mons (MonsID fr, MonsID to);
void eff_mons_sk_levels_up (MonsID mons, Skill sk);
void eff_mons_levels_up (MonsID mons);
void eff_mons_picks_up_item (MonsID mons, ItemID item);
void eff_mons_wields_item (MonsID mons, ItemID item);
void eff_mons_unwields (MonsID mons);
void eff_mons_wears_item (MonsID mons, ItemID item);
void eff_mons_takes_off_item (MonsID mons, ItemID item);
void eff_mons_angers_mons (MonsID fr, MonsID to);
void eff_mons_calms (MonsID mons);

#endif /* PANEL_H_INCLUDED */

