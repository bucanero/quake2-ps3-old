#include "header/local.h"

refimport_t	ri;

/*
===============
RE_RegisterSkin
===============
*/
static struct image_s *
RE_RegisterSkin (char *name)
{
	return R_FindImage (name, it_skin);
}

void R_Printf(int level, const char* msg, ...)
{
	va_list argptr;
	va_start(argptr, msg);
	ri.Com_VPrintf(level, msg, argptr);
	va_end(argptr);
}


/*
============
RE_SetSky
============
*/
// 3dstudio environment map names
static const char	*suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};
static const int	r_skysideimage[6] = {5, 2, 4, 1, 0, 3};
extern	mtexinfo_t		r_skytexinfo[6];

static void
RE_SetSky (char *name, float rotate, vec3_t axis)
{
	Com_Printf("RE_SetSky(%s) not implemented\n", name);
}

/*
================
RE_RenderFrame

================
*/
static void
RE_RenderFrame (refdef_t *fd)
{
    Com_Printf("RE_RenderFrame not implemented\n");
}

/*
===============
R_Init
===============
*/
static qboolean
RE_Init(void)
{
    Com_Printf("RE_Init not implemented\n");
    return false;
}

static qboolean
RE_IsVsyncActive(void)
{
    Com_Printf("RE_IsVsyncActive not implemented\n");
	return true;
}

/*
===============
RE_Shutdown
===============
*/
static void
RE_Shutdown (void)
{
    Com_Printf("RE_Shutdown not implemented\n");
}

static int
RE_InitContext(void *win)
{
    Com_Printf("RE_InitContext not implemented\n");
    return false;
}

static void
RE_ShutdownContext(void)
{
    Com_Printf("RE_ShutdownContext not implemented\n");
}

static int RE_PrepareForWindow(void)
{
	Com_Printf("RE_PrepareForWindow not implemented\n");
	return 0;
}

/*
** RE_SetPalette
*/
static void
RE_SetPalette(const unsigned char *palette)
{
	Com_Printf("RE_SetPalette not implemented\n");
}

/*
** RE_BeginFrame
*/
static void
RE_BeginFrame( float camera_separation )
{
    Com_Printf("RE_BeginFrame not implemented\n");
}

/*
=====================
RE_EndWorldRenderpass
=====================
*/
static qboolean
RE_EndWorldRenderpass( void )
{
	return true;
}

/*
** RE_EndFrame
**
** This does an implementation specific copy from the backbuffer to the
** front buffer.
*/
static void
RE_EndFrame (void)
{
     Com_Printf("RE_EndFrame not implemented\n");
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