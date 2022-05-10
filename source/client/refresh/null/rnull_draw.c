#include "header/local.h"

//=============================================================================

/*
================
RE_Draw_FindPic
================
*/
image_t *
RE_Draw_FindPic (char *name)
{
	Com_Printf("RE_Draw_FindPic(%s) not implemented\n", name);
    return NULL;
}

/*
=============
RE_Draw_GetPicSize
=============
*/
void
RE_Draw_GetPicSize (int *w, int *h, char *name)
{
	image_t *gl;

	gl = RE_Draw_FindPic (name);
	if (!gl)
	{
		*w = *h = -1;
		return;
	}
	*w = gl->asset_width;
	*h = gl->asset_height;
}

/*
=============
Draw_Pic
=============
*/
void
RE_Draw_PicScaled(int x, int y, char *name, float scale)
{
	image_t		*pic;

	pic = RE_Draw_FindPic (name);
	if (!pic)
	{
		R_Printf(PRINT_ALL, "Can't find pic: %s\n", name);
		return;
	}

	Com_Printf("RE_Draw_PicScaled not implemented\n");
}

/*
=============
RE_Draw_StretchPic
=============
*/
void
RE_Draw_StretchPic (int x, int y, int w, int h, char *name)
{
	image_t	*pic;

	pic = RE_Draw_FindPic (name);
	if (!pic)
	{
		R_Printf(PRINT_ALL, "Can't find pic: %s\n", name);
		return;
	}
	Com_Printf("RE_Draw_StretchPic not implemented\n");
}

/*
================
Draw_Char

Draws one 8*8 graphics character
It can be clipped to the top of the screen to allow the console to be
smoothly scrolled off.
================
*/
void
RE_Draw_CharScaled(int x, int y, int c, float scale)
{
    Com_Printf("RE_Draw_CharScaled not implemented\n");
}

/*
=============
RE_Draw_TileClear

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
void
RE_Draw_TileClear (int x, int y, int w, int h, char *name)
{
    Com_Printf("RE_Draw_TileClear not implemented\n");
}

/*
=============
RE_Draw_Fill

Fills a box of pixels with a single color
=============
*/
void
RE_Draw_Fill (int x, int y, int w, int h, int c)
{
    Com_Printf("RE_Draw_Fill not implemented\n");
}

/*
================
RE_Draw_FadeScreen

================
*/
void
RE_Draw_FadeScreen (void)
{
    Com_Printf("RE_Draw_FadeScreen not implemented\n");
}

/*
=============
RE_Draw_StretchRaw
=============
*/
void
RE_Draw_StretchRaw (int x, int y, int w, int h, int cols, int rows, byte *data)
{
    Com_Printf("RE_Draw_StretchRaw not implemented\n");
}
