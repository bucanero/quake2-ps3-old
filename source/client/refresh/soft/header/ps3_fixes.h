#ifndef _PS3_FIXES_H_
#define _PS3_FIXES_H_

#include "local.h"

// Structures to communicate between software
// renderer and it's frontend (SDL or something)
typedef struct sw_context_s
{
    pixel_t	*swap_frames[2];
    viddef_t resolution;

    void (*FlushFrame) (int vmin, int vmax);
    void (*CleanFrame) (void);
} sw_context_t;

typedef struct sw_rend_s
{
    int	*swap_current;

    cvar_t	*sw_partialrefresh;
    cvar_t	*sw_anisotropic;

    void (*NoDamageBuffer) (void);
    void (*CopyFrame) (uint32_t * pixels, int pitch, int vmin, int vmax);
    void (*Free) (void);
} sw_rend_t;


/**
 * \brief Get the number of milliseconds since the first call.
 * \details Replcamenet for SDL_GetTicks()
 *
 * \note This value wraps if the program runs for more than ~49 days.
 */
uint32_t getTicks(void);

#endif
