#if !defined(RANK_H_INCLUDED)
#define      RANK_H_INCLUDED

enum RANKS
{
    RNK_NONE = 0,
    RNK_SOLDIER,
    RNK_DOCTOR
};

char *get_rank(void);

#endif    /* RANK_H_INCLUDED */
