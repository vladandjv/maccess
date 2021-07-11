/**************************************************************************/
/* PROGRAM: MASHMLOCK.C                                                   */
/* DESCRIPTION: Locking system implementation based on shared memory      */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "defs.h"
#include "maccess.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/**************************************************************************/
long long *
DB_Lock_Init(key_t shm_mem_code, size_t shm_mem_size)
{
#ifdef SINGLE_USER_NO_SHARED_MEMORY
  return (0);
#else
  key_t shmid;
  long long *returna;

  shmid = shmget((key_t)shm_mem_code, (size_t)shm_mem_size,
                 IPC_CREAT | IPC_EXCL | 0600);
  if (shmid != -1)
    returna = (long long *)shmat(shmid, NULL, SHM_RND);
  else
  {
    shmid = shmget((key_t)shm_mem_code, (size_t)shm_mem_size,
                   IPC_CREAT | 0600);
    if (shmid == -1)
      logMessage("DB_Lock_Init: Cannot allocate a shared memory segment!");
    returna = (long long *)shmat(shmid, NULL, SHM_RND);
  }
  return (returna);
#endif
}
/**************************************************************************/
void DB_Lock_Close(long long *retrna)
{
#ifdef SINGLE_USER_NO_SHARED_MEMORY
  return;
#else
  if (shmdt(retrna) < 0)
    logMessage("Detach_SHM: Cannot detach from the shared memory segment");
#endif
}
/**************************************************************************/
void SHM_Lock(long long *shm_lock) /* Shared memory lock */
{
#ifdef SINGLE_USER_NO_SHARED_MEMORY
  return;
#else
  while (*shm_lock == 1)
    sleep(1);

  *shm_lock = 1;
#endif
}
/**************************************************************************/
void SHM_UnLock(long long *shm_lock) /* Shared memory unlock */
{
#ifdef SINGLE_USER_NO_SHARED_MEMORY
  return;
#else
  *shm_lock = 0;
#endif
}
/**************************************************************************/
