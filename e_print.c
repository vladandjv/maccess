/*
 * PROGRAM: E_PRINT.C                                                        
 * DESCRIPTION: Example program for Maccess funcitons, without any other
 * libraries. It shows the loaded records in the databse.
 * AUTHOR: VLADAN DJORDJEVIC                                               
 */

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
  void Control(), Next();

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
  ClearKey(&IExt);

  Next();
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
void Next()
{
  char temp[11];
  char Mat[115];
  info = &OurRecord;

  long long i;

  for (i = 1;; i++)
  {
    SHM_Lock();
    NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
    if (OKAY == T)
    {
      GetRec(DPtr, &DExt, TaRecNum, info);
      SHM_UnLock();
      strcpy(Mat, info->Key);
      strcat(Mat, " ");
      strcat(Mat, info->Surname);
      strcat(Mat, " ");
      strcat(Mat, info->Name);
      strcat(Mat, " ");
      strcat(Mat, info->Remark);
      printf("Mat[%10lld] = [%s]\n", i, Mat);
    }
    else
    {
      SHM_UnLock();
      if (i == 1)
        printf("The database is empty! Load it with ./e_load\n");
      break;
    }
  }
}
/**************************************************************************/
void Control()
{
  if (fopen(DatFName, "rb") == NULL)
  {
    printf("There are no files, nothing to print!\nFirst use e_print and e_load respectively.\n");
    exit(0);
  }
}
/**************************************************************************/
