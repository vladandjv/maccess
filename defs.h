/**************************************************************************
 *  DEFS.H
 *	Basic definitions for all source code
 *	Author: Vladan Djordjevic
 **************************************************************************/

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

/* In case the database engine is needed for private data of an 
 * application, then single-user mode is desired and shared     
 * memory is unnecessary.
 */
#define SINGLE_USER_NO_SHARED_MEMORY

#ifndef SINGLE_USER_NO_SHARED_MEMORY
#define LOCK_SHM_MEM_CODE (key_t)121
#endif
#define MACCESS_SHM_MEM_CODE (key_t)122

#define _HAS_FLUSH
/* Flush on a disk after every record added to data base.
 * It is the must in multiuser envirioment with milions of 
 * records! */

#define MAX_FILENAME_LENGHT 64 /* Can be much longer at the expense of memory */

#define APP_LOGFILE "example.log"

/* #define DEBUG_APP */

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

#endif /* _DEFS_H */
