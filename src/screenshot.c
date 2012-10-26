#include <stdint.h>
#include "include/graphics.h"

uint32_t Old_buffer[2000];
extern uint32_t New_buffer[2000], Current_buffer[2000];

/* prototypes in util.h */
void screenshot()
{
	int i;

	gr_refresh();
	for (i = 0; i < 2000; ++i)
	{
		//Old_buffer[i] = Current_buffer[i];
	}
}

void unscreenshot()
{
	int i;

	for (i = 0; i < 2000; ++i)
	{
		//New_buffer[i] = Old_buffer[i];
	}
	gr_refresh();
}
