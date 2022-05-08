// system.c for PSL1GHT

// #include "../../common/header/common.h"
#include "system.h"

#include <stdio.h>
#include <ppu-lv2.h>
#include <lv2/syscalls.h>

#include <sys/file.h>
#include <sys/stat.h>

void 
Sys_Error(char *error, ...)
{
	// FIXME Implement Sys_Quit
}

void Sys_Quit(void)
{
	// FIXME Implement Sys_Quit
}

void 
Sys_Init(void)
{
}


// ---------------------------------------------
//  Input/Output
// ---------------------------------------------

// returns static null terminated string or NULL
char*
Sys_ConsoleInput(void)
{
	/* PS3 had tty input but let's be real
	   we will never use ps3 as dedicated server
	   moreover with tty console input.
	   In future it could be onscreen keyboard
	   call. */
	return NULL;
}

// prints null terminated <string> to stdout
void
Sys_ConsoleOutput(char *string)
{
	fputs(string, stdout);
}


// ---------------------------------------------
//  Time
// ---------------------------------------------

inline void
Sys_getCurrentTime(int64_t* seconds, int64_t* nanoseconds)
{
	lv2syscall2(SYSCALL_TIME_GET_CURRENT_TIME, (uint64_t)seconds, (uint64_t)nanoseconds);
	// printf("Sys_getCurrentTime {s: %ld n: %ld}\n", *seconds, *nanoseconds);
}

#define TIMESPEC_NOW(ts) Sys_getCurrentTime(&(ts.tv_sec), &(ts.tv_nsec))

// returns microseconds since first call
long long
Sys_Microseconds(void)
{
	// Call itself takes around 1-3 us. More on first time.
	// Uses implementation from YQ2's system.c for unix 

	struct timespec now;
	static struct timespec first = {0, 0};
	TIMESPEC_NOW(now);

	if(first.tv_sec == 0)
	{
		long long nsec = now.tv_nsec;
		long long sec = now.tv_sec;
		// set back first by 1ms so neither this function nor Sys_Milliseconds()
		// (which calls this) will ever return 0
		nsec -= 1000000ll;
		if(nsec < 0)
		{
			nsec += 1000000000ll; // 1s in ns => definitely positive now
			--sec;
		}

		first.tv_sec = sec;
		first.tv_nsec = nsec;
	}

	long long sec  = now.tv_sec  - first.tv_sec;
	long long nsec = now.tv_nsec - first.tv_nsec;
	
	return sec*1000000ll + nsec/1000ll;
}

int
Sys_Milliseconds(void)
{
	return (int)(Sys_Microseconds()/1000ll);
}

// sleep for <nanosec> nanoseconds
void 
Sys_Nanosleep(int nanosec)
{
	/* Uses usleep syscall instead of nanosleep
	   due lack of it on PowerPC.
	   It's ain't problem b/c current codebase of YQ2
	   not calling it for precision higher then 1 us. */
	lv2syscall1(SYSCALL_TIMER_USLEEP, (uint64_t)(nanosec/1000ll));
}


// ---------------------------------------------
//  Filesytem
// ---------------------------------------------

#define PS3_PATH_MAX 256

// /absolute/path/to/directory
void 
Sys_Mkdir(const char *path)
{
	// code based on one in apollo's savetool
	
	char fullpath[PS3_PATH_MAX];

	//snprintf(fullpath, sizeof(fullpath), "%s", path);
	strlcpy(fullpath, path, PS3_PATH_MAX);

    char* ptr = fullpath;

	// creating path to directory
	while (*ptr)
    {
    	ptr++;
        while (*ptr && *ptr != '/')
		{
            ptr++;
		}

        char last = *ptr;
		*ptr = 0;

		printf("%s\n", fullpath);
        if (Sys_IsDir(fullpath) == false)
        {
            if (mkdir(fullpath, 0777) < 0)
			{
                return;
			}
            sysLv2FsChmod(fullpath, S_IFDIR | 0777);
        }
        
        *ptr = last;
    }

    return;
}

qboolean
Sys_IsDir(const char *path)
{
	struct stat sb;

	if (stat(path, &sb) != -1)
	{
		if (S_ISDIR(sb.st_mode))
		{
			return true;
		}
	}

	return false;
}

qboolean
Sys_IsFile(const char *path)
{
	struct stat sb;

	if (stat(path, &sb) != -1)
	{
		if (S_ISREG(sb.st_mode))
		{
			return true;
		}
	}

	return false;
}

// returns null terminated string -- path to
// config directory should end with .YQ2
// which is set at <cfgdir> variable and
// defined in macro CFGDIR
char*
Sys_GetHomeDir(void)
{
	// FIXME Implement Sys_GetHomeDir
	return NULL;
}

// See remove(3)
// Used in *download and Load/Save functions
void
Sys_Remove(const char *path)
{
	remove(path);
}

// See rename(2)
// Used in *download functions
int
Sys_Rename(const char *from, const char *to)
{
	return rename(from, to);
}

// removes dir <path> if it is exists
// <path> is absolute
void
Sys_RemoveDir(const char *path)
{
	if (sysLv2FsRmdir(path) != 0) 
	{
		printf("Sys_RemoveDir: can't remove dir: '%s'", path);
	}
}

// writes return of realpath(3) (<in>, NULL)
// to buffer <out> with max size <size>
// if realpath returns NULL returns false
// otherwise returns true
qboolean
Sys_Realpath(const char *in, char *out, size_t size)
{
	// FIXME: realpath not wokring
	return false;
}


#ifdef NEED_GET_PROC_ADDRESS
#error "Sys_GetProcAddress could not be implemented"
// void*
// Sys_GetProcAddress(void *handle, const char *sym);
#endif

#ifndef UNICORE
#error "dynamic libs could not be implemented"
// void
// Sys_FreeLibrary(void *handle);
// void*
// Sys_LoadLibrary(const char *path, const char *sym, void **handle);
// void*
// Sys_GetGameAPI(void *parms);
// void 
// Sys_UnloadGame(void);
#endif


// ---------------------------------------------
//  Process location
// ---------------------------------------------

// writes YQ2's working directory path* 
// to <buffer> up to <len> bytes
// *path should be null terminated
void
Sys_GetWorkDir(char *buffer, size_t len)
{
}

// Sets YQ2's working directory to <path>
// <path> is null terminate string
// returns true on success false on fail
qboolean
Sys_SetWorkDir(char *path)
{
	return false;
}
