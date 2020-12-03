/**************************************************************************/
/* PROGRAM: E1_LOAD.C                                                     */
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
  void Loading (), Control ();

  openLog (APP_LOGFILE);
  signal (SIGSEGV, sig_handler);
  signal (SIGINT, sig_handler);
  if (signal (SIGTERM, sig_handler) == SIG_ERR)
    logMessage ("Can't catch SIGTERM signal");

  Lock = DB_Lock_Init (LOCK_SHM_MEM_CODE_1, LOCK_SHM_MEM_SIZE);
  SHM_Lock (Lock);
  Control (); /* Must be before InitAccess */
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 2);
  OpenIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 3);
  OpenIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 4);
  OpenIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 5);
  SHM_UnLock (Lock);

  printf ("I am working!\n");
  Loading ();
  printf ("DONE!\n");

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
Loading ()
{
  long long z;
  info = &MyRecord;

  //for (z = 1; z <= 100000000; z++) {
  for (z = 1; z <= 1000000; z++)
    {
      memset (info, 0, sizeof (struct Record));
      info->Deleted = 0;
      uuidt (info->client_id);
      prepare_str (info->client_id, 64);
#ifdef DEBUG_APP
      logMessage ("L0 client_id = [%s] %d", info->client_id, strlen (info->client_id));
#endif
      hex_string (info->group_id, 64);
      hex_string (info->resource_id, 64);
      hex_string (info->auth_id, 64);
      strcpy (info->socket_id, "127.0.0.1:4567");
      prepare_str (info->socket_id, 47);
      strcpy (info->status, "1a");
      sprintf (info->timestamp_l, "%010lld", (long long) time (NULL));
      sprintf (info->timestamp_h, "%010lld", (long long) time (NULL));
#ifdef DEBUG_APP
      logMessage ("Iteration= %ld", z);
      logMessage ("info->Deleted = %lld", info->Deleted);
      logMessage ("L client_id = [%s] %d", info->client_id, strlen (info->client_id));
      logMessage ("L group_id = [%s] %d", info->group_id, strlen (info->group_id));
      logMessage ("L resource_id = [%s] %d", info->resource_id, strlen (info->resource_id));
      logMessage ("L auth_id = [%s] %d", info->auth_id, strlen (info->auth_id));
      logMessage ("L auth_id = [%s] %d", info->socket_id, strlen (info->socket_id));
      logMessage ("L status = [%s] %d", info->status, strlen (info->status));
      logMessage ("L timestamp_l = [%s] %d", info->timestamp_l, strlen (info->timestamp_l));
      logMessage ("L timestamp_h = [%s] %d", info->timestamp_h, strlen (info->timestamp_h));
#endif
      SHM_Lock (Lock);
      AddRec (DPtr, &DExt, &TaRecNum, info);
      AddKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) info->client_id);
      AddKey (IPtr_group_id, &IExt_group_id, &TaRecNum, (TaKeyStr *) info->group_id);
      AddKey (IPtr_resource_id, &IExt_resource_id, &TaRecNum, (TaKeyStr *) info->resource_id);
      AddKey (IPtr_auth_id, &IExt_auth_id, &TaRecNum, (TaKeyStr *) info->auth_id);
      AddKey (IPtr_status, &IExt_status, &TaRecNum, (TaKeyStr *) info->status);
      SHM_UnLock (Lock);
    }
}
/**************************************************************************/
void
Control ()
{
  FILE *fp = NULL;

  if ((fp = fopen (DatFName, "OrdNum")) == NULL)
    {
      Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);
      InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
      MakeFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
      MakeIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 1);
      MakeIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 2);
      MakeIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 3);
      MakeIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 4);
      MakeIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 5);
      CloseFile (DPtr, &DExt);
      CloseIndex (IPtr_client_id, &IExt_client_id);
      CloseIndex (IPtr_group_id, &IExt_group_id);
      CloseIndex (IPtr_resource_id, &IExt_resource_id);
      CloseIndex (IPtr_auth_id, &IExt_auth_id);
      CloseIndex (IPtr_status, &IExt_status);
      TermAccess ((long long) 0);
      Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);
    }
  else
    fclose (fp);
}
/**************************************************************************/
/*  The hyphens are removed, and also the MAC address (6byte) is removed.
 *  Three time-based UUIDs are concatenated, and a machine identifier
 *  (1A2B for vladan.boyal.us) is appended to the end. */

void
uuidt (char *uuid2)
{
  uuid_t binuuid;
  char uuid[37]; /* (36 characters), plus '\0' */
  char uuid1[37]; /* (36 characters), plus '\0' */
  int i, j;

  uuid_generate_time (binuuid);
  uuid_unparse (binuuid, uuid);
  for (i = 0, j = 0; i < 23; ++i)
    {
      if (uuid[i] != '-')
        {
          uuid1[j] = uuid[i];
          ++j;
        }
    }
  uuid1[j] = '\0';
  strcpy (uuid2, uuid1);

  uuid_generate_time (binuuid);
  uuid_unparse (binuuid, uuid);
  for (i = 0, j = 0; i < 23; ++i)
    {
      if (uuid[i] != '-')
        {
          uuid1[j] = uuid[i];
          ++j;
        }
    }
  uuid1[j] = '\0';

  strcat (uuid2, uuid1);
  uuid_generate_time (binuuid);
  uuid_unparse (binuuid, uuid);
  for (i = 0, j = 0; i < 23; ++i)
    {
      if (uuid[i] != '-')
        {
          uuid1[j] = uuid[i];
          ++j;
        }
    }
  uuid1[j] = '\0';
  strcat (uuid2, uuid1);
  strcat (uuid2, "1A2B");
}
/**************************************************************************/
void
hex_string (char *str, int length)
{ /* Random hex generator */
  //hexadecimal characters
  char hex_characters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

  int i;
  for (i = 0; i < length; i++)
    {
      str[i] = hex_characters[rand () % 16];
    }
  str[length] = '\0';
}
/**************************************************************************/
void
empty_str (char *str, int length) /* Fille the string with ' ' */
{
  int i;

  for (i = 0; i < length; i++)
    str[i] = ' ';
  str[length] = '\0';
  return;
}
/**************************************************************************/
/* Remove empty strings and add ' ' at the end if string lenght is less
 * then required */
void
prepare_str (char *str, int length)
{
  char blank[MAX_STRING_LENGTH];
  int c, d;

  for (c = 0, d = 0; str[c] != '\0'; c++)
    {
      if (str[c] != ' ')
        {
          blank[d] = str[c];
          break;
        }
    }

  for (; str[c] != '\0'; c++, d++)
    blank[d] = str[c];

  for (; d <= length; d++)
    {
      blank[d] = ' ';
    }
  blank[length] = '\0';
  strcpy (str, blank);

  return;
}
/**************************************************************************/
/* Remove empty carachters at the and of a string */
void
trim_str (char *str)
{
  int c;

  c = strlen (str) - 1;

  for (; str[c] == ' ' && c != '\0'; c--)
    str[c] = '\0';
}
/**************************************************************************/
/* Cut the string from the left */
void
left_str (char *str, int lenght)
{
  if (strlen (str) <= lenght)
    return;
  str[lenght] = '\0';
  return;
}
/****************************** END ***************************************/
