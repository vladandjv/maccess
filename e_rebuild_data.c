/*
 * PROGRAM: E_REBUILD_DATA.C                                                        
 * DESCRIPTION: Rebuilds the data file. It is called within the
 * e_rebuild.sh (bash script).
 * AUTHOR: VLADAN DJORDJEVIC                                               
 */

#include "maccess.h"
#include "example.h"

FileName DatFName = "data.dbc";
FileName DatFName1 = "data.tmp";
unsigned long long KeyLen = 11; /* Should be key lenght + 1 */
struct IndexExt IExt;
struct DataExt DExt;
DataFilePtr IPtr = NULL;
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
  void Control(), RebuildData();

  openLog(APP_LOGFILE);
  signal(SIGSEGV, sig_handler);
  signal(SIGINT, sig_handler);
  if (signal(SIGTERM, sig_handler) == SIG_ERR)
    logMessage("Can't catch SIGTERM signal");
  Control();

  printf("I am working on the data file, first!\n");
  RebuildData();
  printf("DONE!\n");

  closeLog();
  exit(0);
}
/**************************************************************************/
void RebuildData()
{
  long long RecordNumber, RecNum, velic;
  struct Record CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
  InitAccess(MACCESS_SHM_MEM_CODE);
  MakeFile(&DPtr1, &DExt1, DatFName1, sizeof(struct Record), (long long)0);
  CloseFile(DPtr1, &DExt1);
  TermAccess();
  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);

  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr1, &DExt1, DatFName1, sizeof(struct Record), (long long)0);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)1);
  velic = FileLen(DPtr, &DExt) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
  {
    GetRec(DPtr, &DExt, RecordNumber, &CurRec);
    if (CurRec.Deleted == 0)
    {
      CurRec1.Deleted = CurRec.Deleted;
      strcpy(CurRec1.Key, CurRec.Key);
      strcpy(CurRec1.Surname, CurRec.Surname);
      strcpy(CurRec1.Name, CurRec.Name);
      strcpy(CurRec1.Remark, CurRec.Remark);
      AddRec(DPtr1, &DExt1, &RecNum, &CurRec1);
    }
  }
  CloseFile(DPtr, &DExt);
  CloseFile(DPtr1, &DExt1);
  TermAccess();
  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
}
/**************************************************************************/
void Control()
{
  if (fopen(DatFName, "rb") == NULL)
  {
    printf("There are no files to rebuild! Use e_load.\n");
    exit(0);
  }
}
/**************************************************************************/
