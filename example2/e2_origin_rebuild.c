/**************************************************************************/
/* PROGRAM: E2_ORIGIN_RUBUILD.C                                           */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"

#define APP_LOGFILE "example2.log"

FileName DatFName = "origin.dbc";
FileName DatFName1 = "origin.tmp";
FileName IndexFName0 = "origin_record_id.cdx";
FileName IndexFName1 = "origin_origin_name.cdx";
unsigned long long origin_record_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long origin_origin_name_Len = 65; /* Should be key lenght + 1 */
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
  void RebuildIndex (), Control ();

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
  struct Record_origin CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_origin), (long long) 0, (long long) 0);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_origin), (long long) 0, (long long) 0);
  OpenFile (&DPtr_origin, &DExt_origin, DatFName, sizeof (struct Record_origin), (long long) 0, (long long) 1);
  velic = FileLen (DPtr_origin, &DExt_origin) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_origin, &DExt_origin, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          CurRec1.Deleted = CurRec.Deleted;
          strcpy (CurRec1.record_id, CurRec.record_id);
          strcpy (CurRec1.origin_name, CurRec.origin_name);
          CurRec1.allowed = CurRec.allowed;
          AddRec (DPtr1, &DExt1, &RecNum, &CurRec1);
        }
    }
  CloseFile (DPtr_origin, &DExt_origin);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  system ("mv origin.tmp origin.dbc");
  system ("rm -rf origin_record_id.cdx origin_origin_name.cdx");

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeIndex (&IPtr_origin_record_id, &IExt_origin_record_id, IndexFName0, origin_record_id_Len, NoDuplicates, (long long) 0, (long long) 0);
  CloseIndex (IPtr_origin_record_id, &IExt_origin_record_id);
  MakeIndex (&IPtr_origin_origin_name, &IExt_origin_origin_name, IndexFName1, origin_origin_name_Len, NoDuplicates, (long long) 0, (long long) 1);
  CloseIndex (IPtr_origin_origin_name, &IExt_origin_origin_name);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr_origin, &DExt_origin, DatFName, sizeof (struct Record_origin), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_origin_record_id, &IExt_origin_record_id, IndexFName0, origin_record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_origin_origin_name, &IExt_origin_origin_name, IndexFName1, origin_origin_name_Len, Duplicates, (long long) 0, (long long) 2);
  velic = FileLen (DPtr_origin, &DExt_origin) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_origin, &DExt_origin, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          AddKey (IPtr_origin_record_id, &IExt_origin_record_id, &RecordNumber, (TaKeyStr *) CurRec.record_id);
          AddKey (IPtr_origin_origin_name, &IExt_origin_origin_name, &RecordNumber, (TaKeyStr *) CurRec.origin_name);
        }
    }
  CloseFile (DPtr_origin, &DExt_origin);
  CloseIndex (IPtr_origin_record_id, &IExt_origin_record_id);
  CloseIndex (IPtr_origin_origin_name, &IExt_origin_origin_name);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "rb") == NULL)
    {
      printf ("There are no files to rebuild! Use e2_origin_load.\n");
      exit (1);
    }
}
/**************************************************************************/

