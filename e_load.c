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

FileName DatFName = "data.dbc";
FileName IndexFName = "data.cdx";
unsigned long long KeyLen = 11; /* Should be key lenght + 1 */
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
    SHM_UnLock();
    CloseFile(DPtr, &DExt);
    CloseIndex(IPtr, &IExt);
    SHM_UnLock();
    TermAccess((long long)0);
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
#ifndef SINGLE_USER_NO_SHARED_MEMORY
  DB_Lock_Init(SEMAPHORE_CODE);
#endif
  SHM_Lock();
  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  SHM_UnLock();

  printf("I am working!\n");
  Loading();
  printf("DONE!\n");

  SHM_Lock();
  CloseFile(DPtr, &DExt);
  CloseIndex(IPtr, &IExt);
  TermAccess();
  SHM_UnLock();
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
    SHM_Lock();
    AddRec(DPtr, &DExt, &TaRecNum, info);
    AddKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)info->Key);
    SHM_UnLock();
    if ((i % 100000) == 0) /* Just for testing multi user processing */
    {
      printf("Added %lld\n", i);
      SHM_Lock();
      FlushFile(DPtr, &DExt); /* Not necessary, but just in case. */
      FlushIndex(IPtr, &IExt);
      SHM_UnLock();
    }
#ifdef DEBUG_APP
    logMessage("Key[%s] Surname[%s] Name[%s] Remark[%s]",
               info->Key, info->Surname, info->Name, info->Remark);
#endif
  }
  i--;
  logMessage("DONE! I have loaded %lld records of %lld", i, j);
}
/**************************************************************************/
void Control()
{
  if (fopen(DatFName, "rb") == NULL)
  {
    printf("There are no files! Use e_print first.\n");
    exit(0);
  }
}
/**************************************************************************/
