#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"
#include "include/graphics.h"

#define NUM_TABS 3

#define PANEL_CLOSED  0
#define PANEL_SELF    1
#define PANEL_MSG     2
#define PANEL_OPTIONS 3

#define P_MSG_LEN 45

struct Monster;

struct P_msg
{
	glyph msg[P_MSG_LEN];
};

void p_init    ();
void p_pane    (struct Monster *);
void p_amsg    (const char *);
void p_msg     (const char *, ...);
char p_ask     (struct Monster *, const char *, const char *);
char p_lines   (Vector);
void p_anotify  (const char *);
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
	char letter;
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
char p_menuex   (Vector lines, const char *toquit, int max_line_len);
Vector p_formatted(const char *input, int max_line_len);
char p_flines   (const char *);
void p_ffree    (Vector);
int  p_status   (struct Monster *, enum PanelType);
int  p_skills   (struct Monster *, enum PanelType);
void p_mvchoose (struct Monster *, int *, int *, const char *, const char *,
	void (*) (enum P_MV, int, int, int, int));

void show_path_on_overlay (enum P_MV, int, int, int, int);
void show_disc_on_overlay (enum P_MV, int, int, int, int);

char p_getch (struct Monster *player);

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
void eff_mons_fail_throw (struct Monster *mons, struct Item *item);
void eff_item_dissipates (struct Item *item);
void eff_item_absorbed (struct Item *item);
void eff_item_hits_wall (struct Item *item);
void eff_proj_misses_mons (struct Item *item, struct Monster *mons);
void eff_proj_hits_mons (struct Item *item, struct Monster *mons, int damage);
void eff_mons_tiredly_misses_mons (struct Monster *fr, struct Monster *to);
void eff_mons_misses_mons (struct Monster *fr, struct Monster *to);
void eff_mons_just_misses_mons (struct Monster *fr, struct Monster *to);
void eff_mons_hits_mons (struct Monster *fr, struct Monster *to, int damage);
void eff_mons_kills_mons (struct Monster *fr, struct Monster *to);
void eff_mons_sk_levels_up (struct Monster *mons, Skill sk);
void eff_mons_levels_up (struct Monster *mons);
void eff_mons_picks_up_item (struct Monster *mons, struct Item *item);
void eff_mons_wields_item (struct Monster *mons, struct Item *item);
void eff_mons_wears_item (struct Monster *mons, struct Item *item);
void eff_mons_takes_off_item (struct Monster *mons, struct Item *item);
void eff_mons_angers_mons (struct Monster *fr, struct Monster *to);
void eff_mons_calms (struct Monster *mons);

#endif /* PANEL_H_INCLUDED */

