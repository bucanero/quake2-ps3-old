#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifdef true
 #undef true
#endif

#ifdef false
 #undef false
#endif

typedef enum {false, true}  qboolean;

char *Sys_ConsoleInput(void);
void Sys_ConsoleOutput(char *string);
void Sys_Error(char *error, ...);
void Sys_Quit(void);
void Sys_Init(void);
char *Sys_GetHomeDir(void);
void Sys_Remove(const char *path);
int Sys_Rename(const char *from, const char *to);
void Sys_RemoveDir(const char *path);
long long Sys_Microseconds(void);
void Sys_Nanosleep(int);

void Sys_GetWorkDir(char *buffer, size_t len);
qboolean Sys_SetWorkDir(char *path);
qboolean Sys_Realpath(const char *in, char *out, size_t size);

int Sys_Milliseconds(void);
// void Sys_Mkdir(const char *path);
// qboolean Sys_IsDir(const char *path);
// qboolean Sys_IsFile(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_H_ */