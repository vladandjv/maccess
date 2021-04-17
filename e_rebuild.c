/*
 * PROGRAM: E_REBUILD.C                                                        
 * DESCRIPTION: Example program for Maccess funcitons, without any other
 * libraries. It loads 10000 records in the same files with which the 
 * example program works. Use in conjunction with the example to ses results.
 * AUTHOR: VLADAN DJORDJEVIC                                               
 */

#include "maccess.h"
#include "example.h"

#define APP_LOGFILE "example.log"

FileName DatFName = "data.dbc";
FileName DatFName1 = "data.tmp";
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
void
sig_handler (int sig)
{

#ifdef DEBUG_APP
  logMessage ("Cought signal %d in sigint_handler", sig);
#endif

  switch (sig)
    {
    case SIGTERM:
    case SIGSEGV:
      closeLog ();
      exit (0);
    case SIGINT:
      signal (sig, SIG_IGN);
      break;

    default:
      signal (sig, SIG_DFL);
    }
}
/**************************************************************************/
int
main ()
{
  void Control (), RebuildIndex ();

  openLog (APP_LOGFILE);
  signal (SIGSEGV, sig_handler);
  signal (SIGINT, sig_handler);
  if (signal (SIGTERM, sig_handler) == SIG_ERR)
    logMessage ("Can't catch SIGTERM signal");
  Control ();

  printf ("I am working!\n");
  RebuildIndex ();
  printf ("DONE!\n");

  closeLog ();
  exit (0);
}
/**************************************************************************/
void
RebuildIndex ()
{
  long long RecordNumber, RecNum, velic;
  struct Record CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE);
  MakeFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record), (long long) 0, (long long) 0);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE);
  OpenFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 1);
  velic = FileLen (DPtr, &DExt) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr, &DExt, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          CurRec1.Deleted = CurRec.Deleted;
          strcpy (CurRec1.Key, CurRec.Key);
          strcpy (CurRec1.Surname, CurRec.Surname);
          strcpy (CurRec1.Name, CurRec.Name);
          strcpy (CurRec1.Remark, CurRec.Remark);
          AddRec (DPtr1, &DExt1, &RecNum, &CurRec1);
        }
    }
  CloseFile (DPtr, &DExt);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);

  system ("mv data.tmp data.dbc");
  system ("rm -rf data.cdx");

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE);
  MakeIndex (&IPtr, &IExt, IndexFName, KeyLen, NoDuplicates, (long long) 0, (long long) 0);
  CloseIndex (IPtr, &IExt);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE);
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenIndex (&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long) 0, (long long) 1);
  velic = FileLen (DPtr, &DExt) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr, &DExt, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        AddKey (IPtr, &IExt, &RecordNumber, (TaKeyStr *) CurRec.Key);
    }
  CloseIndex (IPtr, &IExt);
  CloseFile (DPtr, &DExt);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "rb") == NULL)
    {
      printf ("There are no files to rebuild! Use e_load.\n");
      exit (1);
    }
}
/**************************************************************************/

