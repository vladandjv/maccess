/*
 * PROGRAM: E_REBUILD_INDEX.C                                                        
 * DESCRIPTION: Rebuilds the index file. It is called within the
 * e_rebuild.sh (bash script).
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
  void Control(), RebuildIndex();

  openLog(APP_LOGFILE);
  signal(SIGSEGV, sig_handler);
  signal(SIGINT, sig_handler);
  if (signal(SIGTERM, sig_handler) == SIG_ERR)
    logMessage("Can't catch SIGTERM signal");
  Control();

  printf("I am working on the index file!\n");
  RebuildIndex();
  printf("DONE!\n");

  closeLog();
  exit(0);
}
/**************************************************************************/
void RebuildIndex()
{
  long long RecordNumber, velic;
  struct Record CurRec;

  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
  InitAccess(MACCESS_SHM_MEM_CODE);
  MakeIndex(&IPtr, &IExt, IndexFName, KeyLen, NoDuplicates, (long long)0);
  CloseIndex(IPtr, &IExt);
  TermAccess();
  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);

  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  velic = FileLen(DPtr, &DExt) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
  {
    GetRec(DPtr, &DExt, RecordNumber, &CurRec);
    if (CurRec.Deleted == 0)
      AddKey(IPtr, &IExt, &RecordNumber, (TaKeyStr *)CurRec.Key);
  }
  CloseIndex(IPtr, &IExt);
  CloseFile(DPtr, &DExt);
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
