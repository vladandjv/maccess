/**************************************************************************/
/* PROGRAM: MAMEMORY.C                                                    */
/* DESCRIPTION: Initialization and management of memory buffers.          */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "mainter.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h> 

#define Size_DF   (sizeof(struct DataFile))  /* used in FindYourPlace */

long long MemId[HasMemories];
char *MemSh[HasMemories];
/**************************************************************************/
void
InitAccess (MemoryNumber, MemoryCode)

long long MemoryNumber;
key_t MemoryCode; /* key_t is defined as long int u /sys/types.h */
{
  void InitPages ();

  OKAY = T;
  IOStatus = 0;
  InitPages (MemoryNumber, MemoryCode);
}
/**************************************************************************/
void
InitPages (MemNum, MemoryCode)

long long MemNum;
key_t MemoryCode;
{
  long long i;

  MemId[MemNum] = StartSM (MemoryCode);
  if (MemId[MemNum] != -1)
    {
      MemSh[MemNum] = USM (MemId[MemNum], MemNum);
      memset (MemSh[MemNum], 0, PageStackSize * sizeof (struct DataFile)
              + sizeof (TaPageStack));
      for (i = 0; i < PageStackSize; i++)
        (*TaPgMap[MemNum])[i] = i;
    }
  else
    {
      MemId[MemNum] = ApproachSM (MemoryCode);
      MemSh[MemNum] = USM (MemId[MemNum], MemNum);
    }
  TaRecBuf = &Buffer;
}
/**************************************************************************/
void
TermAccess (MemNum)

long long MemNum;
{
  LeaveSM (MemSh[MemNum]);
}
/**************************************************************************/
long long
StartSM (MemoryCode)

key_t MemoryCode;
{
  int shmid;

  if (MemoryCode > 999)
    {
      IOStatus = ErrMemoryCode;
      TAIOCheck ();
    }
  shmid = shmget ((key_t) MemoryCode, (size_t) MACCESS_SHM_MEM_SIZE,
                  IPC_CREAT | IPC_EXCL | 0600);
  return (shmid);
}
/**************************************************************************/
long long
ApproachSM (MemoryCode)

key_t MemoryCode;
{
  int shmid;

  if (MemoryCode > 999)
    {
      IOStatus = ErrMemoryCode;
      TAIOCheck ();
    }
  shmid = shmget ((key_t) MemoryCode, (size_t) MACCESS_SHM_MEM_SIZE,
                  IPC_CREAT | 0600);

#ifdef DEBUG
  logMessage ("shmid %d,MemoryCode %d, MACCESS_SHM_MEM_SIZE %ld", shmid, MemoryCode, MACCESS_SHM_MEM_SIZE);
#endif

  if (shmid == -1)
    {
      IOStatus = ErrGetSM;
      TAIOCheck ();
    }
  return (shmid);
}
/**************************************************************************/
char *
USM (shmid, MemNum)

int shmid;
long long MemNum;
{
  char *addr;
  char *retrna;

  addr = (char *) 0;
  retrna = (char *) shmat (shmid, addr, SHM_RND);
  if (retrna == (char *) - 1)
    {
      IOStatus = ErrAtSM;
      TAIOCheck ();
    }
  TaPageStk[MemNum] = (TaPageStackPtr) (retrna + PageStackSize * sizeof (struct DataFile));
  TaPgMap[MemNum] = (TaPageMapPtr) (retrna + PageStackSize * sizeof (struct DataFile)
                                 + sizeof (TaPageStack));
  return (retrna);
}
/***************************************************************************/
long long
FindYourPlace (DatF, MemNum, FileNum)

DataFilePtr *DatF;
long long MemNum;
long long FileNum;
{
  *DatF = (DataFilePtr) (MemSh[MemNum] + FileNum * (long long) Size_DF);
  if ((long long) *(MemSh[MemNum] + FileNum * (long long) Size_DF + sizeof (long long)) == 0)
    return (0);
  else
    return (1);
}
/**************************************************************************/
void
LeaveSM (retrna)

char *retrna;
{
  long long retrn;

  retrn = shmdt (retrna);
  if (retrn == -1)
    {
      IOStatus = ErrDtSM;
      TAIOCheck ();
    }
}
/**************************************************************************/
int
Destroy_SHM (key_t shm_mem_code, size_t shm_mem_size)
{
  key_t shmid;
  struct shmid_ds *buf = NULL;

  /* returns -1 on error, else shmid */
  shmid = shmget ((key_t) shm_mem_code, (size_t) shm_mem_size,
                  IPC_CREAT | 0600);

#ifdef DEBUG
  logMessage ("shmid %d,shm_mem_code %d, shm_mem_size %ld", shmid, shm_mem_code, shm_mem_size);
#endif

  if (shmid < 0)
    return (-1);

  /* 0 is returned on success, -1 on error. */
  if (shmctl (shmid, IPC_RMID, buf) < 0)
    {
      logMessage ("Destroy_SHM: Cannot destroy the shared memory segment!");
      return (-1);
    }

  return (0);
}
/**************************************************************************/
