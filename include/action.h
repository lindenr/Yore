#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

void mons_regen (struct Thing *);
void act_mhit   (struct Thing *, struct Thing *);
void act_mtouch (struct Thing *, struct Thing *);
void act_mclaw  (struct Thing *, struct Thing *);
void act_mbite  (struct Thing *, struct Thing *);

#endif /* ACTION_H_INCLUDED */

