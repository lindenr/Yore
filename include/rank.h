#if !defined(RANK_H_INCLUDED)
#define      RANK_H_INCLUDED

#include "include/monst.h"

enum RANKS
{
    RNK_NONE = 0,
    RNK_SOLDIER,
    RNK_DOCTOR
};

char *get_rank(void);
void  update_level(struct Monster*);

#endif    /* RANK_H_INCLUDED */
