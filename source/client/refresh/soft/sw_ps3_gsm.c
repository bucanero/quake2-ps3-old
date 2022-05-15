#include "header/ps3_local.h"
#include "header/rsxutil.h"
#include <sysutil/video.h>

// -----------------------------------------------------------------------------
//   GCM frontend implementation variables
// -----------------------------------------------------------------------------
static int rend_buffer_width;
static int rend_buffer_height;
static int rend_buffer_pitch;

static rsxBuffer render_buffer;

static gcmContextData* gcmContext;
// -----------------------------------------------------------------------------


// =============================================================================
//   Internal software refresher functions
// =============================================================================

void
RE_FlushFrame(int vmin, int vmax)
{
	uint32_t *pixels = render_buffer.ptr;

	if (sw_partialrefresh->value)
	{
		// Update required part
		RE_CopyFrame(pixels, rend_buffer_pitch / sizeof(uint32_t),
			vmin, vmax);
	}
	else
	{
		RE_CopyFrame(pixels, rend_buffer_pitch / sizeof(uint32_t),
			0, vid_buffer_height * vid_buffer_width);
	}

	if ((sw_anisotropic->value > 0) && !fastmoving)
	{
		SmoothColorImage(pixels + vmin, vmax - vmin, sw_anisotropic->value);
	}

	waitFlip();
	flip(gcmContext, render_buffer.id);

	// replace use next buffer
	swap_current = swap_current + 1;
	vid_buffer = swap_frames[swap_current & 1];

	// All changes flushed
	VID_NoDamageBuffer();
}

void
RE_CleanFrame(void)
{
	memset(swap_frames[0], 0,
		vid_buffer_height * vid_buffer_width * sizeof(pixel_t));
	memset(swap_frames[1], 0,
		vid_buffer_height * vid_buffer_width * sizeof(pixel_t));

	// only cleanup texture without flush texture to screen
	memset(render_buffer.ptr, 0,
		rend_buffer_width * rend_buffer_height * sizeof(uint32_t));

	// All changes flushed
	VID_NoDamageBuffer();
}

/*
** R_GammaCorrectAndSetPalette
*/
/* It is crucial to had it here - different frontends had
   different palettes types */
void
R_GammaCorrectAndSetPalette(const unsigned char *palette)
{
	int i;

	// Replace palette
	for ( i = 0; i < 256; i++ )
	{
		if (sw_state.currentpalette[i*4+0] != sw_state.gammatable[palette[i*4+2]] ||
			sw_state.currentpalette[i*4+1] != sw_state.gammatable[palette[i*4+1]] ||
			sw_state.currentpalette[i*4+2] != sw_state.gammatable[palette[i*4+0]])
		{
			// SDL BGRA
			// sw_state.currentpalette[i*4+0] = sw_state.gammatable[palette[i*4+2]]; // blue
			// sw_state.currentpalette[i*4+1] = sw_state.gammatable[palette[i*4+1]]; // green
			// sw_state.currentpalette[i*4+2] = sw_state.gammatable[palette[i*4+0]]; // red

			// sw_state.currentpalette[i*4+3] = 0xFF; // alpha

			// GCM ARGB
			sw_state.currentpalette[i*4+0] = 0xFF; // alpha
			sw_state.currentpalette[i*4+1] = sw_state.gammatable[palette[i*4+0]]; // red
			sw_state.currentpalette[i*4+2] = sw_state.gammatable[palette[i*4+1]]; // green
			sw_state.currentpalette[i*4+3] = sw_state.gammatable[palette[i*4+2]]; // blue

			palette_changed = true;
		}
	}
}

// =============================================================================
//   Externaly called functions
// =============================================================================

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
		Com_Printf("%s() must not be called with NULL argument!\n",
			__func__);
		ri.Sys_Error(ERR_FATAL,
			"%s() must not be called with NULL argument!", __func__);
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

	vid_buffer_height = vid.height;
	vid_buffer_width = vid.width;

	Com_Printf("vid_buffer_width : %d\n", vid_buffer_width);
	Com_Printf("vid_buffer_height : %d\n", vid_buffer_height);

	makeBuffer(&render_buffer, rend_buffer_width, rend_buffer_height, 0);
	flip(gcmContext, render_buffer.id);

	R_InitGraphics(vid_buffer_width, vid_buffer_height);
	SWimp_CreateRender(vid_buffer_width, vid_buffer_height);

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

	RE_ShutdownRenderer();

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

	Swap_Init();

	return refexport;
}
