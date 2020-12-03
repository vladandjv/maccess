/* defs.h */

#ifndef _DEFS_H
#define _DEFS_H

#include "conf_lib.h"

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
#define min(br1,br2)   ((br1 < br2) ? br1 : br2)
#define max(br1,br2)   ((br1 > br2) ? br1 : br2) 

extern char APP_Program_Name[MAX_STRING_LENGTH];

enum Bool {
    F, T
};
typedef enum Bool Boolean;

#if __cplusplus
extern "C" {
#endif /* __cplusplus */

    char *vsubstr(char *InField, int Poc, int Vel);
    char *left(char *InField, int Vel);
    char *right(char *InField, int Vel);
    char *ltrim(char *InField);
    char *trim(char *InField);
    char *to_right(char *InField, int Vel);
    char *to_left(char *InField, int Vel);
    char *empty_string(int Vel);
    char *ponovi(char Symbol, int Vel);
    int isEmpty(char *InField);
    char *my_date(), *datplus(char *InField, int dani);
    char *datrazl(char *InField1, char *InField2), *okrenidat(char *InField);
    char *my_time(), *time_and_date();
    char *time_difference(char *InField1, char *InField2), *vrmuredi(char *InField);

    /* Shared memory functions */
    int Destroy_SHM(key_t shm_mem_code, size_t shm_mem_size);
    void SHM_Lock(long long *shm_lock), SHM_UnLock(long long *shm_lock);

    /* Locking system functions */
    long long *DB_Lock_Init(key_t shm_mem_code, size_t shm_mem_size);
    void DB_Lock_Close(long long *retrna);

#if __cplusplus
};
#endif /* __cplusplus */

#endif  /* _DEFS_H */
