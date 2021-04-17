/**************************************************************************/
/* PROGRAM: E1_RUBUILD.C                                                  */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example1.h"
#include <string.h>
#include <uuid/uuid.h>
#include <time.h>

#define APP_LOGFILE "example1.log"

FileName DatFName = "data1.dbc";
FileName DatFName1 = "data1.tmp";
FileName IndexFName0 = "client_id.cdx";
FileName IndexFName1 = "group_id.cdx";
FileName IndexFName2 = "resource_id.cdx";
FileName IndexFName3 = "auth_id.cdx";
FileName IndexFName4 = "status.cdx";
unsigned long long client_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long group_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long resource_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long auth_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long status_Len = 3; /* Should be key lenght + 1 */

struct IndexExt IExt_client_id;
DataFilePtr IPtr_client_id = NULL;
struct IndexExt IExt_group_id;
DataFilePtr IPtr_group_id = NULL;
DataFilePtr IPtr_resource_id = NULL;
struct IndexExt IExt_auth_id;
DataFilePtr IPtr_auth_id = NULL;
struct IndexExt IExt_status;
DataFilePtr IPtr_status = NULL;
struct Record *info = NULL;
struct IndexExt IExt;
struct DataExt DExt;
DataFilePtr DPtr = NULL;
struct IndexExt IExt_resource_id;
long long TaRecNum = 0;
struct Record MyRecord;
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
  struct Record CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
  MakeFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record), (long long) 0, (long long) 0);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
  OpenFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 1);
  velic = FileLen (DPtr, &DExt) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr, &DExt, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          CurRec1.Deleted = CurRec.Deleted;
          strcpy (CurRec1.client_id, CurRec.client_id);
          strcpy (CurRec1.group_id, CurRec.group_id);
          strcpy (CurRec1.resource_id, CurRec.resource_id);
          strcpy (CurRec1.auth_id, CurRec.auth_id);
          strcpy (CurRec1.socket_id, CurRec.socket_id);
          strcpy (CurRec1.status, CurRec.status);
          strcpy (CurRec1.timestamp_l, CurRec.timestamp_l);
          strcpy (CurRec1.timestamp_h, CurRec.timestamp_h);
          AddRec (DPtr1, &DExt1, &RecNum, &CurRec1);
        }
    }
  CloseFile (DPtr, &DExt);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);

  system ("mv data1.tmp data1.dbc");
  system ("rm -rf client_id.cdx group_id.cdx \
           resource_id.cdx auth_id.cd status.cdx");

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
  MakeIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 0);
  CloseIndex (IPtr_client_id, &IExt_client_id);
  MakeIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 1);
  CloseIndex (IPtr_group_id, &IExt_group_id);
  MakeIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 2);
  CloseIndex (IPtr_resource_id, &IExt_resource_id);
  MakeIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 3);
  CloseIndex (IPtr_auth_id, &IExt_auth_id);
  MakeIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 4);
  CloseIndex (IPtr_status, &IExt_status);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 2);
  OpenIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 3);
  OpenIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 4);
  OpenIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 5);
  velic = FileLen (DPtr, &DExt) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr, &DExt, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          AddKey (IPtr_client_id, &IExt_client_id, &RecordNumber, (TaKeyStr *) CurRec.client_id);
          AddKey (IPtr_group_id, &IExt_group_id, &RecordNumber, (TaKeyStr *) CurRec.group_id);
          AddKey (IPtr_resource_id, &IExt_resource_id, &RecordNumber, (TaKeyStr *) CurRec.resource_id);
          AddKey (IPtr_auth_id, &IExt_auth_id, &RecordNumber, (TaKeyStr *) CurRec.auth_id);
          AddKey (IPtr_status, &IExt_status, &RecordNumber, (TaKeyStr *) CurRec.status);
        }
    }
  CloseIndex (IPtr_client_id, &IExt_client_id);
  CloseIndex (IPtr_group_id, &IExt_group_id);
  CloseIndex (IPtr_resource_id, &IExt_resource_id);
  CloseIndex (IPtr_auth_id, &IExt_auth_id);
  CloseIndex (IPtr_status, &IExt_status);
  CloseFile (DPtr, &DExt);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "rb") == NULL)
    {
      printf ("There are no files to rebuild! Use e1_load.\n");
      exit (1);
    }
}
/**************************************************************************/

