/**************************************************************************/
/* PROGRAM: E1_PRINT.C                                                    */
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
long long *Lock; /* locks the shared memory segment*/

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
      SHM_UnLock (Lock);
      CloseFile (DPtr, &DExt);
      CloseIndex (IPtr_client_id, &IExt_client_id);
      CloseIndex (IPtr_group_id, &IExt_group_id);
      CloseIndex (IPtr_resource_id, &IExt_resource_id);
      CloseIndex (IPtr_auth_id, &IExt_auth_id);
      CloseIndex (IPtr_status, &IExt_status);
      SHM_UnLock (Lock);
      TermAccess ((long long) 0);
      SHM_UnLock (Lock);
      DB_Lock_Close (Lock);
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
  void Control (), Next ();

  openLog (APP_LOGFILE);
  signal (SIGSEGV, sig_handler);
  signal (SIGINT, sig_handler);
  if (signal (SIGTERM, sig_handler) == SIG_ERR)
    logMessage ("Can't catch SIGTERM signal");

  Lock = DB_Lock_Init (LOCK_SHM_MEM_CODE_1, LOCK_SHM_MEM_SIZE);
  SHM_Lock (Lock);
  Control (); /* Must be before InitAccess */
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
  info = &MyRecord;
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 2);
  OpenIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 3);
  OpenIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 4);
  OpenIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 5);
  SHM_UnLock (Lock);

  Next ();

  SHM_Lock (Lock);
  CloseFile (DPtr, &DExt);
  CloseIndex (IPtr_client_id, &IExt_client_id);
  CloseIndex (IPtr_group_id, &IExt_group_id);
  CloseIndex (IPtr_resource_id, &IExt_resource_id);
  CloseIndex (IPtr_auth_id, &IExt_auth_id);
  CloseIndex (IPtr_status, &IExt_status);
  TermAccess ((long long) 0);
  SHM_UnLock (Lock);
  DB_Lock_Close (Lock);
  closeLog ();
  exit (0);
}
/**************************************************************************/
void
Next ()
{
  char temp[65];
  char Mat[400];
  long long i;
  info = &MyRecord;

  for (i = 1;; i++)
    {
      SHM_Lock (Lock);
      NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          GetRec (DPtr, &DExt, TaRecNum, info);
          SHM_UnLock (Lock);
#ifdef DEBUG_APP
          logMessage ("N client_id = [%s] %d", info->client_id, strlen (info->client_id));
          logMessage ("N group_id = [%s] %d", info->group_id, strlen (info->group_id));
          logMessage ("N resource_id = [%s] %d", info->resource_id, strlen (info->resource_id));
          logMessage ("N auth_id = [%s] %d", info->auth_id, strlen (info->auth_id));
          logMessage ("N socket_id = [%s] %d", info->socket_id, strlen (info->socket_id));
          logMessage ("N status = [%s] %d", info->status, strlen (info->status));
          logMessage ("N timestamp_l = [%s] %d", info->timestamp_l, strlen (info->timestamp_l));
          logMessage ("N timestamp_h = [%s] %d", info->timestamp_h, strlen (info->timestamp_h));
#endif
          strcpy (Mat, info->client_id);
          strcat (Mat, " ");
          strcat (Mat, info->group_id);
          strcat (Mat, " ");
          strcat (Mat, info->resource_id);
          strcat (Mat, " ");
          strcat (Mat, info->auth_id);
          strcat (Mat, " ");
          strcat (Mat, info->socket_id);
          strcat (Mat, " ");
          strcat (Mat, info->status);
          strcat (Mat, " ");
          strcat (Mat, info->timestamp_l);
          strcat (Mat, " ");
          strcat (Mat, info->timestamp_h);
          printf ("Mat[%10lld] = [%s]\n", i, Mat);
#ifdef DEBUG_APP
          logMessage ("Recored print lenght = %ld", strlen (Mat));
#endif
        }
      else
        {
          SHM_UnLock (Lock);
          if (i == 1)
            printf ("The database is empty! Load it with ./e1_load\n");
          break;
        }
    }
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "OrdNum") == NULL)
    {
      SHM_UnLock (Lock);
      printf ("There are no files to print! Use e1_load first.\n");
      exit (1);
    }
}
/**************************************************************************/

