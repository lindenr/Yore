/* pline.c
 * Linden Ralph */

#include "include/all.h"
#include "include/pline.h"
#include "include/loop.h"
#include "include/util.h"
#include <stdio.h>
#include <malloc.h>

int msg_size_pline = 0, line_pline = 0;

bool plined = false;

char pask(const char *in, const char* out, ...)
{
    va_list args;
    char c;
    char o[80];
    int where = 0;
    plined = true;

    /* format the string */
    va_start(args, out);
    vsprintf(o,out,args);
    va_end(args);

    /* make the question */
    where+=strlen(o);
    memcpy(o+where, " (", 2);
    where += 2;
    memcpy(o+where, in, strlen(in));
    where += strlen(in);
    memcpy(o+where, ")", 1);
    where += 1;
    o[where] = '\0'; /* terminating null character */

    /* print the question */
    aline(o);

    /* wait for answer */
    do c = getch();
    while((!is_in(in, c)) && c != ' ' && c != 0x1B);
    return c;
}

void mvline(uint32_t yloc, uint32_t xloc, const char *txt, ...)
{
    char out[30];
    va_list args;
    int i, len;

    va_start(args, txt);
    vsprintf(out, txt, args);
    va_end  (args);
    len = strlen(out);

    if (!(yloc > 0 && yloc < 79 && xloc > 0 && xloc < 79))
    {
        /* not middle */
        return;
    }
    mvaddch(yloc-1, xloc-1, ACS_ULCORNER);
    mvaddch(yloc+1, xloc-1, ACS_LLCORNER);
    mvaddch(yloc-1, xloc,   ACS_HLINE);
    mvaddch(yloc+1, xloc,   ACS_HLINE);
    mvaddch(yloc-1, xloc+1, ACS_URCORNER);
    mvaddch(yloc+1, xloc+1, ACS_LRCORNER);
    if (xloc < 40)
    {
        /* text on RHS */
        mvaddch(yloc,   xloc-1, ACS_VLINE);
        mvaddch(yloc,   ++xloc, ACS_LTEE);
        while (xloc < 40) mvaddch(yloc, ++xloc, ACS_HLINE);
        ++ xloc; mvaddch(yloc, xloc, ACS_RTEE);
        mvprintw(yloc, xloc+1, out);
        mvaddch(yloc-1, xloc, ACS_ULCORNER);
        mvaddch(yloc+1, xloc, ACS_LLCORNER);
        for (i = 0; i < len; ++ i)
        {
            mvaddch(yloc-1, xloc+i+1, ACS_HLINE);
            mvaddch(yloc+1, xloc+i+1, ACS_HLINE);
        }
        mvaddch(yloc-1, xloc+len+1, ACS_URCORNER);
        mvaddch(yloc,   xloc+len+1, ACS_VLINE);
        mvaddch(yloc+1, xloc+len+1, ACS_LRCORNER);
    }
    else
    {
        /* text on LHS */
        mvaddch(yloc,   xloc+1, ACS_VLINE);
        mvaddch(yloc,   --xloc, ACS_RTEE);
        while (xloc >= 40) mvaddch(yloc, --xloc, ACS_HLINE);
        -- xloc; mvaddch(yloc, xloc, ACS_LTEE);
        mvprintw(yloc, xloc-len, out);
        mvaddch(yloc-1, xloc, ACS_URCORNER);
        mvaddch(yloc+1, xloc, ACS_LRCORNER);
        for (i = 0; i < len; ++ i)
        {
            mvaddch(yloc-1, xloc-i-1, ACS_HLINE);
            mvaddch(yloc+1, xloc-i-1, ACS_HLINE);
        }
        mvaddch(yloc-1, xloc-len-1, ACS_ULCORNER);
        mvaddch(yloc,   xloc-len-1, ACS_VLINE);
        mvaddch(yloc+1, xloc-len-1, ACS_LLCORNER);
    }
    move(0,0);
}

void pline(const char* out, ...)
{
    va_list args;
    char *actual = malloc(sizeof(char)*500);
    plined = true;

    va_start(args, out);
    vsprintf(actual, out, args);
    aline((const char *)actual);
    va_end(args);
    free(actual);
}

void aline(const char*out)
{
    int len;
    plined = true;
    reset_col();
    if (strlen(out) > console_width-5)
    {
        /* TODO change */
        out = "pline length exceeded";
    }

    len = strlen(out);
    if (msg_size_pline + len >= console_width-9)
    {
        move(line_pline, msg_size_pline);
        fprintf(stdout, "--more--");
        move(line_pline, msg_size_pline+9);
        msg_size_pline = 0;
        getch();
    }
    if (msg_size_pline == 0) CLEAR_LINE(line_pline);
    move(line_pline, msg_size_pline);
    fprintf(stdout, "%s ", out);
    msg_size_pline += len+1;
    move(line_pline, msg_size_pline);
    refresh();
}

void line_reset()
{
    int i;

    move(line_pline,0);
    for (i = 0; i < console_width; ++ i) fprintf(stdout, " ");    
    msg_size_pline = 0;
}

bool pline_check()
{
    bool ret = plined;
    plined = 0;
    return ret;
}

void mlines(int num_lines, ...)
{
    va_list args;
    struct List list = LIST_INIT;

    va_start(args, num_lines);

    while(num_lines--)
        push_back(&list, va_arg(args, char*));

    va_end(args);

    mlines_list(list, num_lines);
    list_free(&list);
}

void mlines_list(struct List list, int num_lines)
{
    struct list_iter *i;
    int l_no;

    screenshot();
    if (num_lines <= 1) aline(list.beg->data);
    else
    {
        clear_screen();
        for (l_no = 0, i = list.beg; iter_good(i); ++l_no, next_iter(&i))
        {
            mvprintw(l_no, 0, "%s", i->data);
            if (l_no == console_height-2)
            {
                mvprintw(l_no+1, 0, "--more--");
                getch();
                l_no = -1;
            }
        }
        move(console_height-1, 0);
        fprintf(stdout, "--END--");
        move(console_height-1, 7);
    }
    getch();
    move(console_height-1, 0);
    fprintf(stdout, "       ");
}

