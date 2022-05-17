#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include "header/ps3_fixes.h"

// ---------------------------------------------------
//            Internal refresher stuff
// ---------------------------------------------------
void R_InitGraphics(int width,int height);
sw_rend_t SWimp_CreateRender(sw_context_t* n_context);

/*\
 * Other main functions declarations.
 * Those are here just to export
 * them from sw_ps3_main.c
 * and not called frome here
 *
 * Maybe it's better to create separate
 * header for main and sdl functions
 * b/c they never would be used as
 * two separate shared libs.
 * Current state is confusing and
 * shitcode atually
\*/
struct image_s * RE_RegisterSkin (char *name);
void RE_SetSky (char *name, float rotate, vec3_t axis);
void RE_RenderFrame (refdef_t *fd);
qboolean RE_Init (void);
qboolean RE_IsVsyncActive (void);
void RE_Shutdown (void);
void RE_SetPalette(const unsigned char *palette);
void RE_BeginFrame( float camera_separation );
qboolean RE_EndWorldRenderpass (void);
void RE_EndFrame(void);

extern cvar_t* r_vsync;
// ---------------------------------------------------


// ---------------------------------------------------
//   Structs used in communication between front-
//       and back- ends of refresher
// ---------------------------------------------------
static sw_context_t context;
static sw_rend_t sw_renderer;
// ---------------------------------------------------


// ---------------------------------------------------
//   SDL frontend implementation variables
// ---------------------------------------------------
static pixel_t	*swap_buffers = NULL;

static SDL_Window	*window = NULL;
static SDL_Texture	*texture = NULL;
static SDL_Renderer	*renderer = NULL;
// ---------------------------------------------------


// =========================================
//   Internal software refresher functions
// =========================================

static void
RE_FlushFrame(int vmin, int vmax)
{
	int pitch;
	Uint32 *pixels;

	if (SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch))
	{
		Com_Printf("Can't lock texture: %s\n", SDL_GetError());
		return;
	}
	if (sw_renderer.sw_partialrefresh->value)
	{
		sw_renderer.CopyFrame (pixels, pitch / sizeof(Uint32), vmin, vmax);
	}
	else
	{
		// On MacOS texture is cleaned up after render,
		// code have to copy a whole screen to the texture
		sw_renderer.CopyFrame (pixels, pitch / sizeof(Uint32), 0, vid_buffer_height * vid_buffer_width);
	}

	if ((sw_renderer.sw_anisotropic->value > 0) && !fastmoving)
	{
		SmoothColorImage(pixels + vmin, vmax - vmin, sw_renderer.sw_anisotropic->value);
	}

	SDL_UnlockTexture(texture);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	// replace use next buffer
	*(sw_renderer.swap_current) = *(sw_renderer.swap_current) + 1;
	vid_buffer = context.swap_frames[*(sw_renderer.swap_current)&1];

	// All changes flushed
	sw_renderer.NoDamageBuffer();
}

void
RE_CleanFrame(void)
{
	int pitch;
	Uint32 *pixels;

	// memset(swap_buffers, 0,
	// 	vid_buffer_height * vid_buffer_width * sizeof(pixel_t) * 2);

	memset(context.swap_frames[0], 0,
		vid_buffer_height * vid_buffer_width * sizeof(pixel_t));
	memset(context.swap_frames[1], 0,
		vid_buffer_height * vid_buffer_width * sizeof(pixel_t));

	if (SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch))
	{
		Com_Printf("Can't lock texture: %s\n", SDL_GetError());
		return;
	}

	// only cleanup texture without flush texture to screen
	memset(pixels, 0, pitch * vid_buffer_height);
	SDL_UnlockTexture(texture);

	// All changes flushed
	sw_renderer.NoDamageBuffer();
}

// =========================================
//   Externaly called functions
// =========================================

// SDL specifict function to get
// flags for window creation
int RE_PrepareForWindow(void)
{
	int flags = SDL_SWSURFACE;
	return flags;
}

int vid_buffer_height = 0;
int vid_buffer_width = 0;

static int
RE_InitContext(void *win)
{
	char title[40] = {0};

	if(win == NULL)
	{
		ri.Sys_Error(ERR_FATAL, "%s() must not be called with NULL argument!", __func__);
		return false;
	}

	window = (SDL_Window *)win;

	/* Window title - set here so we can display renderer name in it */
	snprintf(title, sizeof(title), "Yamagi Quake II %s - Soft Render", YQ2VERSION);
	SDL_SetWindowTitle(window, title);

	if (r_vsync->value)
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}
	else
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	}

	/* Select the color for drawing. It is set to black here. */
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	/* Clear the entire screen to our selected color. */
	SDL_RenderClear(renderer);

	/* Up until now everything was drawn behind the scenes.
	   This will show the new, black contents of the window. */
	SDL_RenderPresent(renderer);

	context.resolution.height = vid_buffer_height = vid.height;
	context.resolution.width = vid_buffer_width = vid.width;

	texture = SDL_CreateTexture(renderer,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				    SDL_PIXELFORMAT_BGRA8888,
#else
				    SDL_PIXELFORMAT_ARGB8888,
#endif
				    SDL_TEXTUREACCESS_STREAMING,
				    vid_buffer_width, vid_buffer_height);

	R_InitGraphics(vid_buffer_width, vid_buffer_height);

	swap_buffers = malloc(vid_buffer_height * vid_buffer_width * sizeof(pixel_t) * 2);
	if (!swap_buffers)
	{
		ri.Sys_Error(ERR_FATAL, "%s: Can't allocate swapbuffer.", __func__);
		// code never returns after ERR_FATAL
		return false;
	}
	context.swap_frames[0] = swap_buffers;
	context.swap_frames[1] = swap_buffers + vid_buffer_height * vid_buffer_width * sizeof(pixel_t);

	context.FlushFrame = RE_FlushFrame;
	context.CleanFrame = RE_CleanFrame;

	sw_renderer = SWimp_CreateRender(&context);

	return true;
}

void
RE_ShutdownContext(void)
{
	if (swap_buffers)
	{
		free(swap_buffers);
	}
	swap_buffers = NULL;
	vid_buffer = NULL;
	context.swap_frames[0] = NULL;
	context.swap_frames[1] = NULL;

	sw_renderer.Free();

	if (texture)
	{
		SDL_DestroyTexture(texture);
	}
	texture = NULL;

	if (renderer)
	{
		SDL_DestroyRenderer(renderer);
	}
	renderer = NULL;
}

/*
===============
GetRefAPI
===============
*/

refexport_t
GetRefAPI(refimport_t imp)
{
	// struct for save refexport callbacks, copy of re struct from main file
	// used different variable name for prevent confusion and cppcheck warnings
	refexport_t	refexport;

	memset(&refexport, 0, sizeof(refexport_t));
	ri = imp;

	refexport.api_version = API_VERSION;

	refexport.BeginRegistration = RE_BeginRegistration;
	refexport.RegisterModel = RE_RegisterModel;
	refexport.RegisterSkin = RE_RegisterSkin;
	refexport.DrawFindPic = RE_Draw_FindPic;
	refexport.SetSky = RE_SetSky;
	refexport.EndRegistration = RE_EndRegistration;

	refexport.RenderFrame = RE_RenderFrame;

	refexport.DrawGetPicSize = RE_Draw_GetPicSize;

	refexport.DrawPicScaled = RE_Draw_PicScaled;
	refexport.DrawStretchPic = RE_Draw_StretchPic;
	refexport.DrawCharScaled = RE_Draw_CharScaled;
	refexport.DrawTileClear = RE_Draw_TileClear;
	refexport.DrawFill = RE_Draw_Fill;
	refexport.DrawFadeScreen = RE_Draw_FadeScreen;

	refexport.DrawStretchRaw = RE_Draw_StretchRaw;

	refexport.Init = RE_Init;
	refexport.IsVSyncActive = RE_IsVsyncActive;
	refexport.Shutdown = RE_Shutdown;
	refexport.InitContext = RE_InitContext;
	refexport.ShutdownContext = RE_ShutdownContext;
	refexport.PrepareForWindow = RE_PrepareForWindow;

	refexport.SetPalette = RE_SetPalette;
	refexport.BeginFrame = RE_BeginFrame;
	refexport.EndWorldRenderpass = RE_EndWorldRenderpass;
	refexport.EndFrame = RE_EndFrame;

	// Tell the client that we're unsing the
	// new renderer restart API.
	ri.Vid_RequestRestart(RESTART_NO);

	Swap_Init ();

	return refexport;
}
