#ifndef OUTPUT_H_INCLUDED
#define OUTPUT_H_INCLUDED

#include <stdint.h>

extern uint32_t current_buffer[1920], new_buffer[1920];
extern void     update_map(void);
extern void     init_map  (void);
extern void     out_loc   (uint32_t*, uint32_t*);

#endif /* OUTPUT_H_INCLUDED */
