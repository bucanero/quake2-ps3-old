#ifndef _PS3_FIXES_H_
#define _PS3_FIXES_H_

#include "local.h"

/**
 * \brief Get the number of milliseconds since the first call.
 * \details Replcamenet for SDL_GetTicks()
 *
 * \note This value wraps if the program runs for more than ~49 days.
 */
uint32_t getTicks(void);

#endif
