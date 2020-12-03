/**************************************************************************/
/* PROGRAM: E2_AUTH_ID_ALLOWED_RUBUILD.C                                  */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"

#define APP_LOGFILE "example2.log"

FileName DatFName = "auth_id_allowed.dbc";
FileName DatFName1 = "auth_id_allowed.tmp";
FileName IndexFName0 = "auth_id_allowed_record_id.cdx";
FileName IndexFName1 = "auth_id_allowed_auth_id.cdx";
struct Record_auth_id_allowed *info;
unsigned long long auth_id_allowed_record_id_Len = 65; /* Key lenght + 1 */
unsigned long long auth_id_allowed_auth_id_Len = 65; /* Key lenght + 1 */
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
  struct Record_auth_id_allowed CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_auth_id_allowed), (long long) 0, (long long) 0);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_auth_id_allowed), (long long) 0, (long long) 0);
  OpenFile (&DPtr_auth_id_allowed, &DExt_auth_id_allowed, DatFName, sizeof (struct Record_auth_id_allowed), (long long) 0, (long long) 1);
  velic = FileLen (DPtr_auth_id_allowed, &DExt_auth_id_allowed) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_auth_id_allowed, &DExt_auth_id_allowed, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          CurRec1.Deleted = CurRec.Deleted;
          strcpy (CurRec1.record_id, CurRec.record_id);
          strcpy (CurRec1.auth_id, CurRec.auth_id);
          AddRec (DPtr1, &DExt1, &RecNum, &CurRec1);
        }
    }
  CloseFile (DPtr_auth_id_allowed, &DExt_auth_id_allowed);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  system ("mv auth_id_allowed.tmp auth_id_allowed.dbc");
  system ("rm -rf auth_id_allowed_record_id.cdx auth_id_allowed_auth_id.cdx");

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeIndex (&IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id, IndexFName0, auth_id_allowed_record_id_Len, NoDuplicates, (long long) 0, (long long) 0);
  CloseIndex (IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id);
  MakeIndex (&IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id, IndexFName1, auth_id_allowed_auth_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  CloseIndex (IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr_auth_id_allowed, &DExt_auth_id_allowed, DatFName, sizeof (struct Record_auth_id_allowed), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id, IndexFName0, auth_id_allowed_record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id, IndexFName1, auth_id_allowed_auth_id_Len, Duplicates, (long long) 0, (long long) 2);
  velic = FileLen (DPtr_auth_id_allowed, &DExt_auth_id_allowed) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_auth_id_allowed, &DExt_auth_id_allowed, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          AddKey (IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id, &RecordNumber, (TaKeyStr *) CurRec.record_id);
          AddKey (IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id, &RecordNumber, (TaKeyStr *) CurRec.auth_id);
        }
    }
  CloseFile (DPtr_auth_id_allowed, &DExt_auth_id_allowed);
  CloseIndex (IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id);
  CloseIndex (IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "rb") == NULL)
    {
      printf ("There are no files to rebuild! Use e2_auth_id_allowed_load.\n");
      exit (1);
    }
}
/**************************************************************************/

