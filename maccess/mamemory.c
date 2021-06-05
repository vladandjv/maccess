/**************************************************************************/
/* PROGRAM: MAMEMORY.C                                                    */
/* DESCRIPTION: Initialization and management of memory buffers.          */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "maccess.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

long long MemId;
char *MemSh = NULL;
/**************************************************************************/
void
    InitAccess(MemoryCode)

        key_t MemoryCode; /* key_t is defined as long int u /sys/types.h */
{
  void InitPages();

  OKAY = T;
  IOStatus = 0;
  InitPages(MemoryCode);
}
/**************************************************************************/
void
    InitPages(MemoryCode)

        key_t MemoryCode;
{
  long long i;

  MemId = StartSM(MemoryCode);
  if (MemId != -1)
  {
    MemSh = USM(MemId);
    memset(MemSh, 0, PageStackSize * sizeof(struct DataFile) + sizeof(TaPageStack));
    for (i = 0; i < PageStackSize; i++)
      (*TaPgMap)[i] = i;
  }
  else
  {
    MemId = ApproachSM(MemoryCode);
    MemSh = USM(MemId);
  }
  TaRecBuf = &Buffer;
}
/**************************************************************************/
void TermAccess()

{
  LeaveSM(MemSh);
}
/**************************************************************************/
long long
    StartSM(MemoryCode)

        key_t MemoryCode;
{
  long long shmid;

  if (MemoryCode > 999)
  {
    IOStatus = ErrMemoryCode;
    TAIOCheck();
  }
  shmid = shmget((key_t)MemoryCode, (size_t)MACCESS_SHM_MEM_SIZE,
                 IPC_CREAT | IPC_EXCL | 0600);
  return (shmid);
}
/**************************************************************************/
long long
    ApproachSM(MemoryCode)

        key_t MemoryCode;
{
  long long shmid;

  if (MemoryCode > 999)
  {
    IOStatus = ErrMemoryCode;
    TAIOCheck();
  }
  shmid = shmget((key_t)MemoryCode, (size_t)MACCESS_SHM_MEM_SIZE,
                 IPC_CREAT | 0600);

#ifdef DEBUG
  logMessage("shmid %d,MemoryCode %d, MACCESS_SHM_MEM_SIZE %ld",
             shmid, MemoryCode, MACCESS_SHM_MEM_SIZE);
#endif

  if (shmid == -1)
  {
    IOStatus = ErrGetSM;
    TAIOCheck();
  }
  return (shmid);
}
/**************************************************************************/
char *
    USM(shmid)

        long long shmid;
{
  char *addr;
  char *retrna;

  /* Attach to the exsisting shared memory segment */
  addr = (char *)0;
  retrna = (char *)shmat(shmid, addr, SHM_RND);
  if (retrna == (char *)-1)
  {
    IOStatus = ErrAtSM;
    TAIOCheck();
  }
  /* Put TaPageStack and TaPgMap in shared memory segment */
  TaPageStk = (TaPageStackPtr)(retrna + PageStackSize * sizeof(struct DataFile));
  TaPgMap = (TaPageMapPtr)(retrna + PageStackSize * sizeof(struct DataFile) + sizeof(TaPageStack));
  return (retrna);
}
/***************************************************************************/
long long
    FindYourPlace(DatF, FileNum)

        DataFilePtr *DatF;
long long FileNum;
{
  *DatF = (DataFilePtr)(MemSh + FileNum * (long long)sizeof(struct DataFile));
  if ((long long)*(MemSh + FileNum * (long long)sizeof(struct DataFile) + sizeof(long long)) == 0)
    return (0);
  else
    return (1);
}
/**************************************************************************/
void
    LeaveSM(retrna)

        char *retrna;
{
  long long retrn;

  retrn = shmdt(retrna);
  if (retrn == -1)
  {
    IOStatus = ErrDtSM;
    TAIOCheck();
  }
}
/**************************************************************************/
long long
Destroy_SHM(key_t shm_mem_code, size_t shm_mem_size)
{
  key_t shmid;
  struct shmid_ds *buf = NULL;

  /* returns -1 on error, else shmid */
  shmid = shmget((key_t)shm_mem_code, (size_t)shm_mem_size, 0600);

#ifdef DEBUG
  logMessage("shmid %d,shm_mem_code %d, shm_mem_size %ld",
             shmid, shm_mem_code, shm_mem_size);
#endif

  if (shmid < 0)
    return (-1);

  /* 0 is returned on success, -1 on error. */
  if (shmctl(shmid, IPC_RMID, buf) < 0)
  {
    logMessage("Destroy_SHM: Cannot destroy the shared memory segment!");
    return (-1);
  }

  return (0);
}
/**************************************************************************/