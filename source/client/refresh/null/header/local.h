#ifndef __R_LOCAL__
#define __R_LOCAL__

#include "../../ref_shared.h"

#define NUM_MIPS 4

typedef struct image_s
{
	char		name[MAX_QPATH];	// game path, including extension
	imagetype_t	type;
	int		width, height;
	int		asset_width, asset_height;	// asset texture size
	qboolean	transparent;		// true if any 255 pixels in image
	int		registration_sequence;  // 0 = free
	byte		*pixels[NUM_MIPS];	// mip levels
	int		mip_levels; // count of mip levels
} image_t;

#include "model.h"

void RE_BeginRegistration (char *model);
struct model_s	*RE_RegisterModel (char *name);
void RE_EndRegistration (void);

struct image_s *RE_Draw_FindPic (char *name);

void RE_Draw_GetPicSize (int *w, int *h, char *name);
void RE_Draw_PicScaled (int x, int y, char *name, float scale);
void RE_Draw_StretchPic (int x, int y, int w, int h, char *name);
void RE_Draw_StretchRaw (int x, int y, int w, int h, int cols, int rows, byte *data);
void RE_Draw_CharScaled (int x, int y, int c, float scale);
void RE_Draw_TileClear (int x, int y, int w, int h, char *name);
void RE_Draw_Fill (int x, int y, int w, int h, int c);
void RE_Draw_FadeScreen (void);

#endif