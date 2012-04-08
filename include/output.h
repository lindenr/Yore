#ifndef OUTPUT_H_INCLUDED
#define OUTPUT_H_INCLUDED

#include <stdint.h>

extern uint32_t current_buffer[1920], new_buffer[1920];

void update_map(void);
void init_map  (void);
void out_loc   (uint32_t*, uint32_t*);

#endif /* OUTPUT_H_INCLUDED */
