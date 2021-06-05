/* defs.h */

#ifndef _DEFS_H
#define _DEFS_H

#if INTPTR_MAX == INT64_MAX
#define _FILE_OFFSET_BITS 64 /* in case lseek is used */
#define _LARGEFILE64_SOURCE
#elif INTPTR_MAX == INT32_MAX
#error This code is optimized and works only on a 64 bits machine.
#else
#error Unknown pointer size or missing size macros!
#endif /* INT64_MAX or INT32_MAX */

#define min(br1, br2) ((br1 < br2) ? br1 : br2)
#define max(br1, br2) ((br1 > br2) ? br1 : br2)

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include "log.h"

#define MAX_STRING_LENGTH 512

/* There is no memory leak - Garbage Collector is only optional */
/* #define WORK_WITH_GARBAGE_COLLECTOR */
#ifdef WORK_WITH_GARBAGE_COLLECTOR
#include "gc.h"
#define malloc(n) GC_malloc(n)
#define realloc(m, n) GC_REALLOC(m, n)
#define free(n) GC_free(n)
#endif /* WORK_WITH_GARBAGE_COLLECTOR */

#define MAX_FILENAME_LENGHT 64 /* Can be much longer at the expense of memory */

/* LOCK_SHM_MEM_SIZE = sizeof(long long) */
#define LOCK_SHM_MEM_SIZE (size_t)16 /* 8 but just in case */
/* MACCESS_SHM_MEM_SIZE = (PageSize * sizeof(struct DataFile) + 
   sizeof(TaPageStack) + sizeof(TaPageMap) + sizeof(long long))  */
#define MACCESS_SHM_MEM_SIZE (size_t)4000000 /* 3639360 but better to be more */

#define LOCK_SHM_MEM_CODE (key_t)121
#define MACCESS_SHM_MEM_CODE (key_t)122

#define _HAS_FLUSH
/* Flush on a disk after every record added to data base.
 * It is the must in multiuser envirioment with milions of 
 * records! */

enum Bool
{
    F,
    T
};
typedef enum Bool Boolean;

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

#endif /* _DEFS_H */
