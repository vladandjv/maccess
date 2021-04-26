/* conf_lib.h */

#ifndef _CONF_LIB_H_
#define _CONF_LIB_H_

/*  #define WORK_WITH_GARBAGE_COLLECTOR  */
#ifdef WORK_WITH_GARBAGE_COLLECTOR
#include "gc.h"
#define malloc(n) GC_malloc(n)
#define realloc(m,n) GC_REALLOC(m,n) 
#define free(n) GC_free(n) 
#endif /* WORK_WITH_GARBAGE_COLLECTOR */

/* LOCK_SHM_MEM_SIZE = sizeof(long long) */
#define LOCK_SHM_MEM_SIZE (size_t)16 /* 8 but just in case */
/* MACCESS_SHM_MEM_SIZE = (PageSize * sizeof(struct DataFile) + 
   sizeof(TaPageStack) + sizeof(TaPageMap) + sizeof(long long))  */
//#define MACCESS_SHM_MEM_SIZE    (size_t) 300000 /* 243728 better to be more */
#define MACCESS_SHM_MEM_SIZE    (size_t) 30000000 /* 5532008 better to be more */

#define LOCK_SHM_MEM_CODE (key_t)121
#define MACCESS_SHM_MEM_CODE (key_t)122   /* for example.c */
#define LOCK_SHM_MEM_CODE_1 (key_t)123
#define MACCESS_SHM_MEM_CODE_1 (key_t)124 /* for example1.c */
#define LOCK_SHM_MEM_CODE_2 (key_t)125
#define MACCESS_SHM_MEM_CODE_2 (key_t)126 /* for example2.c */

#define _IMA_FLUSH  
/* Flush on a disk after every record added to data base.
 * It is the must in multiuser envirioment with milions of 
 * records! */

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

/* #define DEBUG */

#endif /*  _CONF_LIB_H_ */
