/* curses-win.c
 * Linden Ralph */

#include "all.h"
#include "mycurses.h"

#if defined(WINDOWS)

#include "pline.h"
#include "loop.h"
#include "thing.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <windows.h>
#include <wincon.h>

HANDLE wHnd;    /* Handle to write to the console.  */
HANDLE rHnd;    /* Handle to read from the console. */

uint32_t Current_buffer[2000], New_buffer[2000];//, Old_buffer[2000];

struct Thing cursor = {THING_CURS, 0, 0, 0};

uint32_t to_buffer(uint32_t y, uint32_t x)
{
    return (((y<<2)+y)<<4)+x;
}

void move(uint32_t y, uint32_t x)
{
    cursor.yloc = y;
    cursor.xloc = x;

    COORD c = (COORD){(short)(x), (short)(y)};
    SetConsoleCursorPosition(wHnd, c);
}

void addch(uint32_t ch)
{
    uint32_t w = to_buffer(cursor.yloc, cursor.xloc);
    New_buffer[w] = ch;
	++cursor.xloc;
	if (cursor.xloc == 80)
	{
		if (cursor.yloc == 24) --cursor.xloc;
		else cursor.xloc = 0;
	}
}

void mvaddch(uint32_t y, uint32_t x, uint32_t ch)
{
    cursor.yloc = y;
    cursor.xloc = x;

    addch(ch);
}

uint32_t tout_num = 0;
void in_tout(uint32_t n)
{
	tout_num = n;
}

void vprintw(const char *str, va_list args)
{
    char buffer[200];
	uint32_t i;
    uint32_t length;
    uint32_t buf = to_buffer(cursor.yloc, cursor.xloc);

    vsprintf(buffer, str, args);
    length = strlen(buffer);
    if(length + buf >= 2000) length = 2000-buf;
    for(i = 0; i < length; ++ i)
        New_buffer[buf+i] = buffer[i];
    move((buf+length)/80, (buf+length)%80);
}

void printw(const char *str, ...)
{
    va_list args;

    va_start(args, str);
    vprintw(str, args);
    va_end(args);
}

void mvprintw(uint32_t y, uint32_t x, const char *str, ...)
{
    va_list args;

    move(y,x);
    va_start(args, str);
    vprintw(str, args);
    va_end(args);
}

void full_refresh()
{
	int i;

    for (i = 0; i < 2000; ++ i)
    {
        Current_buffer[i] = '/';
        New_buffer[i] = ' ';
//		Old_buffer[i] = ' ';
    }

    refresh();
}

void initscr()
{
    /* Set up the handles for reading/writing: */
    wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
    rHnd = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(wHnd, 0);

    SMALL_RECT windowSize = {0, 0, 79, 24};
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

    COORD bufferSize = {80, 25};
    SetConsoleScreenBufferSize(wHnd, bufferSize);

    /* Initialise the buffers. */
    full_refresh();
}

/* Just a placeholder */
void noecho(){}
void echo(){}
void endwin(){}

#define GET_COLOUR_ATTR(x) ((x)>>8)
#define COLOUR_DEF         (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define COLOUR_BRIGHT      (COLOUR_DEF | FOREGROUND_INTENSITY)

WORD colSan(WORD w)
{
	if (!w) return COLOUR_DEF;
    if (w == 65535) return FOREGROUND_RED;
	return w;
}

void set_colour(WORD col)
{
	col = colSan(col);
	SetConsoleTextAttribute(wHnd, col);
}

void write_con(uint32_t *n, char *s, uint32_t len)
{
    DWORD d;
	uint32_t i, cur;
	WORD col;
	if (!len) return;

	col = GET_COLOUR_ATTR(n[0]);
	set_colour(col);
	for (i = 1, cur = 0; i < len; ++ i)
	{
		if (colSan(GET_COLOUR_ATTR(n[i])) == colSan(col)) continue;
		WriteConsole(wHnd, &s[cur], i-cur, &d, NULL);
		cur = i;
		col = GET_COLOUR_ATTR(n[cur]);
		set_colour(col);
	}
	WriteConsole(wHnd, &s[cur], i-cur, &d, NULL);
}

void refresh()
{
    uint32_t prevx = cursor.xloc, prevy = cursor.yloc, y, x;
    for(y = 0; y < 25; ++ y)
    {
        uint32_t ybuf = to_buffer(y,0);
        uint32_t firstc = -1u, lastc = 0;
        char line[80];
        for(x = 0; x < 80; ++ x)
        {
            if(New_buffer[ybuf + x] != Current_buffer[ybuf+x])
            {
                if(firstc == -1u) firstc = x;
                lastc = x;
                Current_buffer[ybuf+x] = New_buffer[ybuf+x];
            };
            if (firstc != -1u) line[x-firstc]=Current_buffer[ybuf+x];
        }
        if (firstc == -1u) continue;
		if (lastc) ++lastc;
        move(y,firstc);
        write_con(&(Current_buffer[ybuf+firstc]), line, lastc-firstc);
    }
    move(prevy, prevx);
}

char KEYS[] = {'h', 'k', 'l', 'j'};
char getch()
{
    refresh();
    char ret;
    DWORD n;
    INPUT_RECORD i;
    clock_t end = tout_num*CLOCKS_PER_SEC/1000 + clock();

    try_again:
    do
    {
        Sleep(20);
        PeekConsoleInput(rHnd, &i, 1, &n);
		if (n==0 && tout_num) continue;
        ReadConsoleInput(rHnd, &i, 1, &n);
    }
    while(i.EventType != KEY_EVENT && clock() < end);
	if(clock() >= end && tout_num) return EOF;

	KEY_EVENT_RECORD k = i.Event.KeyEvent;
	if((k.bKeyDown == TRUE) && (k.wRepeatCount > 0) && (k.wVirtualKeyCode > 0)
	   && ((k.wVirtualKeyCode >= 0x30 && k.wVirtualKeyCode <= 0x5a)
		   || k.wVirtualKeyCode == VK_RETURN || k.wVirtualKeyCode == VK_BACK
		   || k.uChar.AsciiChar == ':'       || k.uChar.AsciiChar == ','
		   || k.uChar.AsciiChar == ' '       || k.uChar.AsciiChar == '.'))
		ret = k.uChar.AsciiChar;
	else if (k.bKeyDown == TRUE && 0x24 < k.wVirtualKeyCode && k.wVirtualKeyCode < 0x29)
		ret = KEYS[k.wVirtualKeyCode-0x25];
	else goto try_again;
    return ret;
}

void wait_ms(unsigned n)
{
	Sleep(n);
}

char *getstr(char *str)
{
	uint32_t i, max = 80;

	for (i = 0; i < max-1;)
	{
		char in = getch();
		if (in == '\b') /* backspace */
		{
			if (i == 0) continue;
			move(cursor.yloc, cursor.xloc-1);
			addch(' ');
			move(cursor.yloc, cursor.xloc-1);
			-- i;
		}
		else if (in == '\n' || in == '\r') break;
		else
		{
			addch(in);
			str[i] = in;
			++ i;
		}
	}
	str[i] = '\0';
	return str;
}

#endif // WINDOWS
