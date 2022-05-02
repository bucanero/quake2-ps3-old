#ifndef RW_PS2
#include <stdint.h>
#endif

#ifdef RW_PS2
	typedef char int8;
	typedef short int16;
	typedef int int32;
	typedef long long int64;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
	typedef unsigned long long uint64;
	typedef unsigned int uintptr;
#else
	/* get rid of the stupid _t */
	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;
	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;
	typedef uintptr_t uintptr;
#endif

typedef float float32;
typedef int32 bool32;
typedef uint8 byte;
typedef uint32 uint;