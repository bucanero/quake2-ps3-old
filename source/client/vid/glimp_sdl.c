#include "../../common/header/common.h"

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>



static cvar_t *vid_displayrefreshrate;
static cvar_t *vid_displayindex;
static cvar_t *vid_rate;

static SDL_Window* window = NULL;
static qboolean initSuccessful = false;
static char **displayindices = NULL;
static int num_displays = 0;

#ifndef Com_Printf
#define Com_Printf printf
#endif























































static void
ClearDisplayIndices(void)
{
	if ( displayindices )
	{
		for ( int i = 0; i < num_displays; i++ )
		{
			free( displayindices[ i ] );
		}

		free( displayindices );
		displayindices = NULL;
	}
}

















































































































































































static void
InitDisplayIndices()
{
	displayindices = malloc((num_displays + 1) * sizeof(char *));

	for ( int i = 0; i < num_displays; i++ )
	{
		/* There are a maximum of 10 digits in 32 bit int + 1 for the NULL terminator. */
		displayindices[ i ] = malloc(11 * sizeof( char ));
		YQ2_COM_CHECK_OOM(displayindices[i], "malloc()", 11 * sizeof( char ))

		snprintf( displayindices[ i ], 11, "%d", i );
	}

	/* The last entry is NULL to indicate the list of strings ends. */
	displayindices[ num_displays ] = 0;
}

/*
 * Lists all available display modes.
 */
static void
PrintDisplayModes(void)
{
	int curdisplay = window ? SDL_GetWindowDisplayIndex(window) : 0;

	// On X11 (at least for me)
	// curdisplay is always -1.
	// DG: probably because window was NULL?
	if (curdisplay < 0) {
		curdisplay = 0;
	}

	int nummodes = SDL_GetNumDisplayModes(curdisplay);

	if (nummodes < 1)
	{
		Com_Printf("Can't get display modes: %s\n", SDL_GetError());
		return;
	}

	for (int i = 0; i < nummodes; i++)
	{
		SDL_DisplayMode mode;

		if (SDL_GetDisplayMode(curdisplay, i, &mode) != 0)
		{
			Com_Printf("Can't get display mode: %s\n", SDL_GetError());
			return;
		}

		Com_Printf(" - Mode %2i: %ix%i@%i\n", i, mode.w, mode.h, mode.refresh_rate);
	}
}


/*
 * Initializes the SDL video subsystem. Must
 * be called before anything else.
 */
qboolean
GLimp_Init(void)
{
	// vid_displayrefreshrate = Cvar_Get("vid_displayrefreshrate", "-1", CVAR_ARCHIVE);
	// vid_displayindex = Cvar_Get("vid_displayindex", "0", CVAR_ARCHIVE);
	// vid_rate = Cvar_Get("vid_rate", "-1", CVAR_ARCHIVE);

	if (!SDL_WasInit(SDL_INIT_VIDEO))
	{
		if (SDL_Init(SDL_INIT_VIDEO) == -1)
		{
			Com_Printf("Couldn't init SDL video: %s.\n", SDL_GetError());

			return false;
		}

		SDL_version version;

		SDL_GetVersion(&version);
		Com_Printf("-------- vid initialization --------\n");
		Com_Printf("SDL version is: %i.%i.%i\n", (int)version.major, (int)version.minor, (int)version.patch);
		Com_Printf("SDL video driver is \"%s\".\n", SDL_GetCurrentVideoDriver());


		num_displays = SDL_GetNumVideoDisplays();
		InitDisplayIndices();
		// ClampDisplayIndexCvar();
		Com_Printf("SDL display modes:\n");

		PrintDisplayModes();
		Com_Printf("------------------------------------\n\n");
	}

    ClearDisplayIndices();
    Com_Printf("ClearDisplayIndices complete\n");

	return true;
}
