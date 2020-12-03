/**************************************************************************/
/* PROGRAM: MASHMLOCK.C                                                   */
/* DESCRIPTION: Locking system implementation based on shared memory      */ 
/* AUTHOR: VLADAN DJORDJEVICH                                              */
/**************************************************************************/
#include "defs.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 

/**************************************************************************/
long long *
DB_Lock_Init (key_t shm_mem_code, size_t shm_mem_size)
{
  key_t shmid;
  long long *returna;

  shmid = shmget ((key_t) shm_mem_code, (size_t) shm_mem_size,
                  IPC_CREAT | IPC_EXCL | 0600);
  if (shmid != -1)
    returna = (long long *) shmat (shmid, NULL, SHM_RND);
  else
    {
      shmid = shmget ((key_t) shm_mem_code, (size_t) shm_mem_size,
                      IPC_CREAT | 0600);
      if (shmid == -1)
        {
          printf ("DB_Lock_Init: Cannot allocate a shared memory segment!\n");
        }
      returna = (long long *) shmat (shmid, NULL, SHM_RND);
    }
  return (returna);
}
/**************************************************************************/
void
DB_Lock_Close (long long *retrna)
{
  if (shmdt (retrna) < 0)
    logMessage ("Detach_SHM: Cannot detach from the shared memory segment");
}
/**************************************************************************/
void
SHM_Lock (long long *shm_lock) /* Shared memory lock */
{
  while (*shm_lock == 1)
    sleep (1 / 1000);
  *shm_lock = 1;
}
/**************************************************************************/
void
SHM_UnLock (long long *shm_lock) /* Shared memory unlock */
{
  *shm_lock = 0;
}
/**************************************************************************/

