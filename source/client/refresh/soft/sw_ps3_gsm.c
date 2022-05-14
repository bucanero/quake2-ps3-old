#include "header/ps3_fixes.h"
#include "header/rsxutil.h"
#include <sysutil/video.h>

// ---------------------------------------------------
//            Internal refresher stuff
// ---------------------------------------------------
static pixel_t	*swap_buffers = NULL;
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
//   GCM frontend implementation variables
// ---------------------------------------------------
static int rend_buffer_width;
static int rend_buffer_height;
static int rend_buffer_pitch;

static rsxBuffer render_buffer;

static gcmContextData* gcmContext;
// ---------------------------------------------------


// =========================================
//   Internal software refresher functions
// =========================================

static void
RE_FlushFrame(int vmin, int vmax)
{
	uint32_t *pixels = render_buffer.ptr;

	if (sw_renderer.sw_partialrefresh->value)
	{
		// Copy previus buffer to current
		// Update required part
		sw_renderer.CopyFrame(pixels, rend_buffer_pitch / sizeof(uint32_t), vmin, vmax);
	}
	else
	{
		// On MacOS texture is cleaned up after render,
		// code have to copy a whole screen to the texture
		sw_renderer.CopyFrame(pixels, rend_buffer_pitch / sizeof(uint32_t), 0, vid_buffer_height * vid_buffer_width);
	}

	if ((sw_renderer.sw_anisotropic->value > 0) && !fastmoving)
	{
		SmoothColorImage(pixels + vmin, vmax - vmin, sw_renderer.sw_anisotropic->value);
	}

	waitFlip();
	flip(gcmContext, render_buffer.id);

	// replace use next buffer
	*(sw_renderer.swap_current) = *(sw_renderer.swap_current) + 1;
	vid_buffer = context.swap_frames[*(sw_renderer.swap_current)&1];

	// All changes flushed
	sw_renderer.NoDamageBuffer();
}

void
RE_CleanFrame(void)
{
	memset(swap_buffers, 0,
		vid_buffer_height * vid_buffer_width * sizeof(pixel_t) * 2);

	// only cleanup texture without flush texture to screen
	memset(render_buffer.ptr, 0,
		rend_buffer_width * rend_buffer_height * sizeof(uint32_t));

	// All changes flushed
	sw_renderer.NoDamageBuffer();
}

// =========================================
//   Externaly called functions
// =========================================

// S.D.L. specifict function to get
// flags for window creation
int RE_PrepareForWindow(void)
{
	return 0;
}

int vid_buffer_height = 0;
int vid_buffer_width = 0;

static int
RE_InitContext(void *gcmCon)
{
	if (gcmCon == NULL)
	{
		Com_Printf("%s() must not be called with NULL argument!\n", __func__);
		ri.Sys_Error(ERR_FATAL, "%s() must not be called with NULL argument!", __func__);
		// FIXME unmapped memory on that
		return false;
	}

	gcmContext = (gcmContextData*)gcmCon;

	if (r_vsync->value)
	{
		gcmSetFlipMode(GCM_FLIP_VSYNC);
	}


	videoConfiguration vConfig;
	if (videoGetConfiguration(0, &vConfig, NULL) != 0)
	{
		Com_Printf("Can't get video configuration\n");
	}
	else
	{
		videoResolution res;
		videoGetResolution(vConfig.resolution, &res);
		rend_buffer_width  = res.width;
	 	rend_buffer_height = res.height;
		rend_buffer_pitch  = vConfig.pitch;
	}

	context.resolution.height = vid_buffer_height = vid.height;
	context.resolution.width = vid_buffer_width = vid.width;

	Com_Printf("vid_buffer_width : %d\n", vid_buffer_width);
	Com_Printf("vid_buffer_height : %d\n", vid_buffer_height);

	makeBuffer(&render_buffer, rend_buffer_width, rend_buffer_height, 0);
	flip(gcmContext, render_buffer.id);

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
	gcmSetWaitFlip(gcmContext);
	memset(render_buffer.ptr, 0,
		rend_buffer_width * rend_buffer_height * sizeof(uint32_t));
	flip(gcmContext, render_buffer.id);
	waitFlip();

	rsxFree(render_buffer.ptr);

	if (swap_buffers)
	{
		free(swap_buffers);
	}
	swap_buffers = NULL;

	vid_buffer = NULL;
	context.swap_frames[0] = NULL;
	context.swap_frames[1] = NULL;

	sw_renderer.Free();

	gcmContext = NULL;
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
