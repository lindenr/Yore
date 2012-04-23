/* curses-linux.c
 * Linden Ralph */

#include "include/mycurses.h"

#ifdef FOONIX

#include "include/all.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

const int ACTUAL_COLOURS[8] = {1, 31, 32, 34, 41, 42, 44, 4}; /* Numbers for ^[[X;Y;Zm. */

uint32_t Current_buffer[2000], New_buffer[2000];
struct termios tm_old;
uint32_t curs_x = 0, curs_y = 0;
int console_width, console_height;

uint32_t to_buffer(uint32_t y, uint32_t x)
{
    return (((y<<2)+y)<<4)+x;
}

void reset_col()
{
    fprintf(stdout, "%c[0m", CHR_ESC);
}

void move(uint32_t y, uint32_t x)
{
    curs_y = y;
    curs_x = x;
    printf("\x1b[%d;%dH", y+1, x+1);
}

void addch(uint32_t ch)
{
    uint32_t w = to_buffer(curs_y, curs_x);
    New_buffer[w] = ch;
}

void mvaddch(uint32_t y, uint32_t x, uint32_t ch)
{
    move(y,x);
    addch(ch);
}

void vprintw(const char *str, va_list args)
{
    char buffer[200];
    uint32_t length;
    uint32_t buf = to_buffer(curs_y, curs_x);
    uint32_t i;

    vsprintf(buffer, str, args);
    length = strlen(buffer);
    if(length + buf >= 2000) length = 2000-buf;
    for(i = 0; i < length; ++ i)
        New_buffer[buf+i] = buffer[i];
    move((buf+length)/80, (buf+length)%80);
}

uint32_t tout_num = 0;
void in_tout(uint32_t n)
{
    tout_num = n;
    toutify(n!=0);
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

void clear_screen()
{
    int i;
    for(i = 0; i < 2000; ++ i)
    {
        New_buffer[i] = ' ';
        Current_buffer[i] = ';';
    }
}

void surf_clear(unsigned size)
{
    int i;

    move(0,0);
    for (i = 0; i < size; ++ i)
        fprintf(stdout, " ");
}

void initscr()
{
    struct winsize W;
    int i;
    
    /* get the size of the screen */
    ioctl(open("/dev/stdout", O_NONBLOCK), TIOCGWINSZ, &W);
    
    /* initialise the buffers */
    for (i = 0; i < 2000; ++ i) New_buffer[i] = Current_buffer[i] = ' ';

    /* clear the *whole* screen (not just the 80x25 portion we work with) */
    console_width = W.ws_col;
    console_height = W.ws_row;

    surf_clear(((unsigned)console_width) * console_height);

    /* save whatever attributes you currently have */
    tcgetattr(STDIN_FILENO, &tm_old);
}

void noecho()
{
    struct termios tm;
    tcgetattr(STDIN_FILENO, &tm);
    tm.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
}

void echo()
{
    struct termios tm;
    tcgetattr(STDIN_FILENO, &tm);
    tm.c_lflag |= ICANON|ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
}

void toutify(int n)
{
    struct termios tm;
    tcgetattr(STDIN_FILENO, &tm);
    tm.c_cc[VMIN] = !n;
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
}

void endwin()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &tm_old);
    printf("%cc", CHR_ESC);
}

#define GET_COLOUR_ATTR(x) ((x)>>8)
#define COLOUR_BRIGHT      (COLOUR_DEF | FOREGROUND_INTENSITY)

uint32_t colSan(uint32_t w)
{
    if (!w) return GET_COLOUR_ATTR(COL_TXT_DEF);
    if (w == (uint32_t)-1) return 1;
    return w;
}

void set_colour(uint32_t col)
{
    col = colSan(col);
    printf("%c[%dm", CHR_ESC, col);
}

void write_con(uint32_t *n, char *s, uint32_t len)
{
    int i;
    s[len] = 0;
    for (i = 0; s[i]; ++ i)
    {
        uint32_t u = n[i]>>8;
        printf("%c[0m", CHR_ESC);
        if(u&1) printf("%c[%dm", CHR_ESC, ACTUAL_COLOURS[0]);
        u >>= 1;
        if (u&7)
            printf("%c[%dm", CHR_ESC, 30+(u&7));
        u >>= 3;
        if (u&8) printf("%c[%dm", CHR_ESC, ACTUAL_COLOURS[7]);
        printf("%c", s[i]);
    }
}

void refresh()
{
    uint32_t prevx = curs_x, prevy = curs_y, y, x;
    for(y = 0; y < 25; ++ y)
    {
        uint32_t ybuf = to_buffer(y,0);
        uint32_t firstc = -1u, lastc = 0;
        char line[81];
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
        if (firstc == -1u) firstc = 0;
        if (lastc)
        {
            ++ lastc;
            move(y,firstc);
            write_con(&(Current_buffer[ybuf+firstc]), line, lastc-firstc);
        }
    }
    reset_col();
    move(prevy, prevx);
}

char getch()
{
    char ret;
    clock_t end;

    refresh();
    if (!tout_num) return getchar();

    end = tout_num*CLOCKS_PER_SEC/1000 + clock();
    do ret = getchar();
    while(clock() < end && ret == EOF);

    return ret;
}

char *getstr(char *str)
{
    char *res;

    res = fgets(str, 80, stdin);
    if (res != str) /* error'd! */
        panic("READ FAILED.");

    str[strlen(str)-1] = '\0';
    return str;
}

#endif /* FOONIX */
