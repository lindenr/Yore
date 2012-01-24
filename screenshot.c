#include <stdint.h>
#include "mycurses.h"

uint32_t Old_buffer[2000];
extern uint32_t New_buffer[2000], Current_buffer[2000];

void screenshot(void)
{
	int i;

	refresh();
    for (i = 0; i < 2000; ++ i)
    {
		Old_buffer[i] = Current_buffer[i];
    }
}

void unscreenshot(void)
{
	int i;

    for (i = 0; i < 2000; ++ i)
    {
		New_buffer[i] = Old_buffer[i];
    }
	refresh();
}

