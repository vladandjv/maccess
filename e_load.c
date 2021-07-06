/**************************************************************************/
/* PROGRAM: E_LOAD.C                                                      */
/* DESCRIPTION: Example program for Maccess funcitons, without any other  */
/* libraries. It loads 1000000 records in the same files with which the   */
/* example program works. Use in conjunction with the example or e_print  */
/* to see results.                                                        */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/

#include "maccess.h"
#include "example.h"

#define APP_LOGFILE "example.log"

FileName DatFName = "data.dbc";
FileName IndexFName = "data.cdx";
unsigned long long KeyLen = 11; /* Should be key lenght + 1 */
long long *Lock;                /* locks the shared memory segment*/
struct IndexExt IExt;
struct DataExt DExt;
DataFilePtr IPtr = NULL;
struct Record OurRecord;
struct Record *info = NULL;
DataFilePtr DPtr = NULL;
long long TaRecNum = 0;

/**************************************************************************/
void sig_handler(int sig)
{

#ifdef DEBUG_APP
  logMessage("Cought signal %d in sigint_handler", sig);
#endif

  switch (sig)
  {
  case SIGTERM:
  case SIGSEGV:
    SHM_UnLock(Lock);
    CloseFile(DPtr, &DExt);
    CloseIndex(IPtr, &IExt);
    SHM_UnLock(Lock);
    TermAccess((long long)0);
    DB_Lock_Close(Lock);
    closeLog();
    exit(0);
  case SIGINT:
    signal(sig, SIG_IGN);
    break;

  default:
    signal(sig, SIG_DFL);
  }
}
/**************************************************************************/
int main()
{
  void Control(), Loading();

  openLog(APP_LOGFILE);
  signal(SIGSEGV, sig_handler);
  signal(SIGINT, sig_handler);
  if (signal(SIGTERM, sig_handler) == SIG_ERR)
    logMessage("Can't catch SIGTERM signal");

  Control(); /* Must be before InitAccess */
  Lock = DB_Lock_Init(LOCK_SHM_MEM_CODE, LOCK_SHM_MEM_SIZE);
  SHM_Lock(Lock);
  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  SHM_UnLock(Lock);

  printf("I am working!\n");
  Loading();
  printf("DONE!\n");

  SHM_Lock(Lock);
  CloseFile(DPtr, &DExt);
  CloseIndex(IPtr, &IExt);
  TermAccess();
  SHM_UnLock(Lock);
  DB_Lock_Close(Lock);
  closeLog();
  exit(0);
}
/**************************************************************************/
void Loading()
{
  info = &OurRecord;
  long long i, j = 1000000;

  logMessage("I am loading %lld records", j);
  info->Deleted = 0;
  for (i = 1; i <= j; ++i)
  {
    sprintf(info->Key, "%010lld", i);
    sprintf(info->Surname, "%025lld", i);
    sprintf(info->Name, "%020lld", i);
    sprintf(info->Remark, "%045lld", i);
    SHM_Lock(Lock);
    AddRec(DPtr, &DExt, &TaRecNum, info);
    AddKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)info->Key);
    SHM_UnLock(Lock);
    if ((i % 100000) == 0) /* Just for testing multi user processing */
    {
      printf("Added %lld\n", i);
#ifndef SINGLE_USER_NO_SHARED_MEMORY
      sleep(2);
#endif
    }
#ifdef DEBUG_APP
    logMessage("Key[%s] Surname[%s] Name[%s] Remark[%s]",
               info->Key, info->Surname, info->Name, info->Remark);
#endif
  }
  SHM_Lock(Lock);
  FlushFile(DPtr, &DExt);
  FlushIndex(IPtr, &IExt);
  SHM_UnLock(Lock);
  i--;
  logMessage("DONE! I have loaded %lld records of %lld", i, j);
}
/**************************************************************************/
void Control()
{
  if (fopen(DatFName, "rb") == NULL)
  {
    printf("There are no files! Use e_print first.\n");
    exit(1);
  }
}
/**************************************************************************/
