#include "header/local.h"

/*
=====================
RE_BeginRegistration

Specifies the model that will be used as the world
=====================
*/
void
RE_BeginRegistration (char *model)
{
    Com_Printf("RE_BeginRegistration(%s) not implemented\n", model);
}

/*
=====================
RE_RegisterModel

=====================
*/
struct model_s *
RE_RegisterModel (char *name)
{
	Com_Printf("RE_RegisterModel(%s) not implemented\n", name);
    return NULL;
}

/*
=====================
RE_EndRegistration

=====================
*/
void
RE_EndRegistration (void)
{
	Com_Printf("RE_EndRegistration not implemented\n");
}