/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * This file implements the low level part of the Hunk_* memory system
 *
 * =======================================================================
 */

/* For mremap() - must be before sys/mman.h include! */
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#if defined(__RSX__)
#define MALLOC_HUNK
#endif

#ifndef MALLOC_HUNK
#include <sys/mman.h>
#endif
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#include "../../common/common.h"

#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <machine/param.h>
#define MAP_ANONYMOUS MAP_ANON
#endif

#if defined(__APPLE__)
#include <sys/types.h>
#define MAP_ANONYMOUS MAP_ANON
#endif

byte *membase;
int maxhunksize;
int curhunksize;

/* TODO: Move ifdef MALLOC_HUNK it to platform independent source file*/ 
void* Hunk_Begin(int maxsize)
{
#ifdef MALLOC_HUNK
	maxhunksize = maxsize;
	curhunksize = 0;
	membase = malloc(maxhunksize);
	if (membase == NULL)
	{
		Sys_Error("unable to allocate %d bytes", maxsize);
	}
	*((int *)membase) = curhunksize;
	return membase;
#else  
	/* reserve a huge chunk of memory, but don't commit any yet */
	maxhunksize = maxsize + sizeof(int);
	curhunksize = 0;

	membase = mmap(0, maxhunksize, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if ((membase == NULL) || (membase == (byte *)-1))
	{
		Sys_Error("unable to virtual allocate %d bytes", maxsize);
	}

	*((int *)membase) = curhunksize;

	return membase + sizeof(int);
#endif
}

void* Hunk_Alloc(int size)
{
	/* round to cacheline */
	size = (size + 31) & ~31;
	if (curhunksize + size > maxhunksize)
		Sys_Error("Hunk_Alloc overflow");
#ifdef MALLOC_HUNK
	byte *buf = membase + curhunksize;
#else
	byte *buf = membase + sizeof(int) + curhunksize;
#endif
	curhunksize += size;
	return buf;
}

int Hunk_End()
{
	byte *n = NULL;

#ifdef MALLOC_HUNK
	n = realloc(membase, curhunksize);
#else
	#if defined(__linux__)
	n = (byte *)mremap(membase, maxhunksize, curhunksize + sizeof(int), 0);
	#elif defined(__FreeBSD__)
	size_t old_size = maxhunksize;
	size_t new_size = curhunksize + sizeof(int);
	void *unmap_base;
	size_t unmap_len;

	new_size = round_page(new_size);
	old_size = round_page(old_size);

	if (new_size > old_size)
	{
		n = 0; /* error */
	}
	else
	if (new_size < old_size)
	{
		unmap_base = (caddr_t)(membase + new_size);
		unmap_len = old_size - new_size;
		n = munmap(unmap_base, unmap_len) + membase;
	}
	#else
	#ifndef round_page
#define round_page(x) \
	(((size_t)(x) + (page_size - 1)) / page_size) * \
	page_size
	#endif

	size_t old_size = maxhunksize;
	size_t new_size = curhunksize + sizeof(int);
	void *unmap_base;
	size_t unmap_len;
	long page_size;

	page_size = sysconf(_SC_PAGESIZE);
	if (page_size == -1)
	{
		Sys_Error("Hunk_End: sysconf _SC_PAGESIZE failed (%d)", errno);
	}

	new_size = round_page(new_size);
	old_size = round_page(old_size);

	if (new_size > old_size)
	{
		n = 0; /* error */
	}
	else
	if (new_size < old_size)
	{
		unmap_base = (caddr_t)(membase + new_size);
		unmap_len = old_size - new_size;
		n = munmap(unmap_base, unmap_len) + membase;
	}
	#endif
#endif

	/* Must be unreachable? Failsafe? */
	if (n != membase)
	{
#ifdef MALLOC_HUNK
		Sys_Error("Hunk_End: Could not reallocate block (%d)", errno);
	}
#else
		Sys_Error("Hunk_End: Could not remap virtual block (%d)", errno);
	}

	*((int *)membase) = curhunksize + sizeof(int);
#endif

	return curhunksize;
}

void Hunk_Free(void *base)
{
	#ifdef MALLOC_HUNK
		if (base)
			free(base);
	#else
	byte *m;

	if (base)
	{
		m = ((byte *)base) - sizeof(int);

		if (munmap(m, *((int *)m)))
		{
			Sys_Error("Hunk_Free: munmap failed (%d)", errno);
		}
	}
	#endif
}
