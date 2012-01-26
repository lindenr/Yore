#ifndef MYCURSES_H_INCLUDED
#define MYCURSES_H_INCLUDED

#include "all.h"
#define CHR_ESC 0x1b

#include <stdint.h>
void move      (uint32_t, uint32_t);
void addch     (uint32_t);
void mvaddch   (uint32_t,uint32_t,uint32_t);
void printw    (const char *,...);
void mvprintw  (uint32_t,uint32_t,const char *,...);
void initscr   (void);
void noecho    (void);
void echo      (void);
void endwin    (void);
void refresh   (void);
char getch     (void);
char*getstr    (char *);

#define CLR(n) ((n)<<8)

#if defined(FOONIX)

#define COL_TXT_RED    CLR(1<<1)
#define COL_TXT_GREEN  CLR(1<<2)
#define COL_TXT_BLUE   CLR(1<<3)
#define COL_TXT_BRIGHT CLR(1)    /* This is actually bold. */
#define COL_TXT_DEF    CLR(COL_TXT_RED|COL_TXT_GREEN|COL_TXT_BLUE)
#define COL_BG_RED     CLR(1<<4)
#define COL_BG_GREEN   CLR(1<<5)
#define COL_BG_BLUE    CLR(1<<6)
#define COL_BG_BRIGHT  CLR(1<<7) /* This is actually underline. */

#define ACS_BULLET   '.'
#define ACS_VLINE    '|'
#define ACS_HLINE    '-'
#define ACS_ULCORNER '-'
#define ACS_LLCORNER '-'
#define ACS_URCORNER '-'
#define ACS_LRCORNER '-'
#define ACS_LTEE     '-'
#define ACS_RTEE     '-'
#define ACS_TTEE     '-'
#define ACS_BTEE     '-'
#define ACS_PLUS     '-'

#elif defined(WINDOWS)

#include <windows.h>

#define COL_TXT_RED    CLR(FOREGROUND_RED)
#define COL_TXT_GREEN  CLR(FOREGROUND_GREEN)
#define COL_TXT_BLUE   CLR(FOREGROUND_BLUE)
#define COL_TXT_BRIGHT CLR(FOREGROUND_INTENSITY)
#define COL_TXT_DEF    CLR(COL_TXT_RED|COL_TXT_GREEN|COL_TXT_BLUE)
#define COL_BG_RED     CLR(FOREGROUND_RED)
#define COL_BG_GREEN   CLR(FOREGROUND_GREEN)
#define COL_BG_BLUE    CLR(FOREGROUND_BLUE)
#define COL_BG_BRIGHT  CLR(BACKGROUND_INTENSITY)

#define ACS_BULLET   0xfa
#define ACS_VLINE    0xb3
#define ACS_HLINE    0xc4
#define ACS_ULCORNER 0xda
#define ACS_LLCORNER 0xc0
#define ACS_URCORNER 0xbf
#define ACS_LRCORNER 0xd9
#define ACS_LTEE     0xc3
#define ACS_RTEE     0xb4
#define ACS_TTEE     0xc2
#define ACS_BTEE     0xc1
#define ACS_PLUS     0xc5

#endif

#endif /* MYCURSES_H_INCLUDED */
