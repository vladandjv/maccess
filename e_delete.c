/*************************************************************************/
/* PROGRAM: E_DELETE.C                                                   */
/* DESCRIPTION: Example program for Maccess funcitons, without any other */
/* libraries. It deletes 999998 records in the same files with which the */
/* example program works. Use in conjunction with the example or e_print */
/* to see results.                                                       */
/* AUTHOR: VLADAN DJORDJEVIC                                             */
/*************************************************************************/

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
  void Control(), Deleting();

  openLog(APP_LOGFILE);
  signal(SIGSEGV, sig_handler);
  signal(SIGINT, sig_handler);
  if (signal(SIGTERM, sig_handler) == SIG_ERR)
    logMessage("Can't catch SIGTERM signal");

  Lock = DB_Lock_Init(LOCK_SHM_MEM_CODE, LOCK_SHM_MEM_SIZE);
  SHM_Lock(Lock);
  Control(); /* Must be before InitAccess */
  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  SHM_UnLock(Lock);
  ClearKey(&IExt);

  printf("I am working!\n");
  Deleting();
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
void Deleting()
{
  long long n = 0;
  long long z = 2;      /*start */
  long long j = 999999; /*end */
  char temp[11];

  n = j - z + 1;
  logMessage("I am deleting %lld records, if they exist.", n);
  for (n = 0; z <= j; ++z)
  {
    sprintf(temp, "%010lld", z);
    SHM_Lock(Lock);
    FindKey(IPtr, &IExt, &TaRecNum, temp);
    if (OKAY == T)
    {
      DeleteRec(DPtr, &DExt, TaRecNum);
      DeleteKey(IPtr, &IExt, &TaRecNum, temp);
      FlushFile(DPtr, &DExt);
      FlushIndex(IPtr, &IExt);
      n++;
    }
    SHM_UnLock(Lock);
    if ((n % 100000) == 0 && n != 0) /* Just for testing multi user processing */
    {
      printf("Deleted %lld\n", n);
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
  {
    long long RecordNumber,
        velic;
    struct Record CurRec;

    if (remove(IndexFName) == -1)
      printf("Error!\n");

    Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
    InitAccess(MACCESS_SHM_MEM_CODE);
    MakeIndex(&IPtr, &IExt, IndexFName, KeyLen, NoDuplicates, (long long)0);
    CloseIndex(IPtr, &IExt);
    OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
    OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
    velic = FileLen(DPtr, &DExt) - 1;
    for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec(DPtr, &DExt, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        AddKey(IPtr, &IExt, &RecordNumber, (TaKeyStr *)CurRec.Key);
    }
  }
  SHM_UnLock(Lock);
  logMessage("DONE! I have deleted %lld records.", n);
}
/**************************************************************************/
void Control()
{
  if (fopen(DatFName, "rb") == NULL)
  {
    printf("There are no files, nothing to delete!\nFirst use e_create and \
    e_load respectively.\n");
    exit(1);
  }
}
/**************************************************************************/
