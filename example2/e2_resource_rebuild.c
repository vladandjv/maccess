/**************************************************************************/
/* PROGRAM: E2_RESOURCE_RUBUILD.C                                         */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"

#define APP_LOGFILE "example2.log"

FileName DatFName = "resource.dbc";
FileName DatFName1 = "resource.tmp";
FileName IndexFName0 = "resource_record_id.cdx";
FileName IndexFName1 = "resource_resource_id.cdx";
struct Record_resource *info;
unsigned long long resource_record_id_Len = 65; /* Key lenght + 1 */
unsigned long long resource_resource_id_Len = 32; /* Key lenght + 1 */
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
  struct Record_resource CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_resource), (long long) 0, (long long) 0);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_resource), (long long) 0, (long long) 0);
  OpenFile (&DPtr_resource, &DExt_resource, DatFName, sizeof (struct Record_resource), (long long) 0, (long long) 1);
  velic = FileLen (DPtr_resource, &DExt_resource) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_resource, &DExt_resource, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          CurRec1.Deleted = CurRec.Deleted;
          strcpy (CurRec1.record_id, CurRec.record_id);
          strcpy (CurRec1.resource_id, CurRec.resource_id);
          strcpy (CurRec1.bash_script, CurRec.bash_script);
          strcpy (CurRec1.special_use, CurRec.special_use);
          AddRec (DPtr1, &DExt1, &RecNum, &CurRec1);
        }
    }
  CloseFile (DPtr_resource, &DExt_resource);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  system ("mv resource.tmp resource.dbc");
  system ("rm -rf resource_record_id.cdx resource_resource_id.cdx");

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeIndex (&IPtr_resource_record_id, &IExt_resource_record_id, IndexFName0, resource_record_id_Len, NoDuplicates, (long long) 0, (long long) 0);
  CloseIndex (IPtr_resource_record_id, &IExt_resource_record_id);
  MakeIndex (&IPtr_resource_resource_id, &IExt_resource_resource_id, IndexFName1, resource_resource_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  CloseIndex (IPtr_resource_resource_id, &IExt_resource_resource_id);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr_resource, &DExt_resource, DatFName, sizeof (struct Record_resource), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_resource_record_id, &IExt_resource_record_id, IndexFName0, resource_record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_resource_resource_id, &IExt_resource_resource_id, IndexFName1, resource_resource_id_Len, Duplicates, (long long) 0, (long long) 2);
  velic = FileLen (DPtr_resource, &DExt_resource) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_resource, &DExt_resource, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          AddKey (IPtr_resource_record_id, &IExt_resource_record_id, &RecordNumber, (TaKeyStr *) CurRec.record_id);
          AddKey (IPtr_resource_resource_id, &IExt_resource_resource_id, &RecordNumber, (TaKeyStr *) CurRec.resource_id);
        }
    }
  CloseFile (DPtr_resource, &DExt_resource);
  CloseIndex (IPtr_resource_record_id, &IExt_resource_record_id);
  CloseIndex (IPtr_resource_resource_id, &IExt_resource_resource_id);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "rb") == NULL)
    {
      printf ("There are no files to rebuild! Use e2_resource_load.\n");
      exit (1);
    }
}
/**************************************************************************/

