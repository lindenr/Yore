#define DEBUGGING

#if defined(DEBUGGING)

#if !defined(DEBUG_H_INCLUDED)
#define DEBUG_H_INCLUDED

enum DEBUG_ERROR_REASON
{
	DEBUG_NO_REASON = 0,
	DEBUG_PLAYER_NOT_FOUND
};

void debug_init(const char *);
void debug_end (void);
void debug_log_error(enum DEBUG_ERROR_REASON, const char *);

#endif /* DEBUG_H_INCLUDED */

#endif /* DEBUGGING */