/**************************************************************************/
/* PROGRAM: E2_IP_ADDRESS_LOAD.C                                          */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"
#include <string.h>
#include <uuid/uuid.h>

#define APP_LOGFILE "example2.log"

FileName DatFName = "ip_address.dbc";
FileName DatFName1 = "ip_address.tmp";
FileName IndexFName0 = "ip_address_record_id.cdx";
FileName IndexFName1 = "ip_address_ip4_add.cdx";
FileName IndexFName2 = "ip_address_ip_allowed.cdx";
struct Record_ip_address *info;
unsigned long long record_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long ip4_add_Len = 17; /* Should be key lenght + 1 */
unsigned long long allowed_Len = 2; /* Should be key lenght + 1 */
long long *Lock; /* locks the shared memory segment*/
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
      CloseFile (DPtr_ip_address, &DExt_ip_address);
      CloseIndex (IPtr_ip_address_record_id, &IExt_ip_address_record_id);
      CloseIndex (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add);
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

  Lock = DB_Lock_Init (LOCK_SHM_MEM_CODE_2, LOCK_SHM_MEM_SIZE);
  SHM_Lock (Lock);
  Control (); /* Must be before InitAccess */
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr_ip_address, &DExt_ip_address, DatFName, sizeof (struct Record_ip_address), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_ip_address_record_id, &IExt_ip_address_record_id, IndexFName0, record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, IndexFName1, ip4_add_Len, Duplicates, (long long) 0, (long long) 2); /* NoDuplicates for the server */
  OpenIndex (&IPtr_ip_address_allowed, &IExt_ip_address_allowed, IndexFName2, allowed_Len, Duplicates, (long long) 0, (long long) 3);
  SHM_UnLock (Lock);

  printf ("I am working!\n");
  Loading ();
  printf ("DONE!\n");

  SHM_Lock (Lock);
  CloseFile (DPtr_ip_address, &DExt_ip_address);
  CloseIndex (IPtr_ip_address_record_id, &IExt_ip_address_record_id);
  CloseIndex (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add);
  CloseIndex (IPtr_ip_address_allowed, &IExt_ip_address_allowed);
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
  info = &R_ip_address;

  //for (z = 1; z <= 100000000; z++) {
  for (z = 1; z <= 100000; z++)
    {
      memset (info, 0, sizeof (struct Record_ip_address));
      info->Deleted = 0; /* not necessary, because of above */
      uuidt (info->record_id);
      prepare_str (info->record_id, 64);
      strcpy (info->ip4_add, "127.0.0.1");
      prepare_str (info->ip4_add, 16);
      strcpy (info->allowed, "1");
      ;
      prepare_str (info->allowed, 1);
#ifdef DEBUG_APP
      logMessage ("L2 Iteration= %ld", z);
      logMessage ("L2 info->Deleted = %lld", info->Deleted);
      logMessage ("L2 record_id = [%s] %d", info->record_id, strlen (info->record_id));
      logMessage ("L2 ip4_add = [%s] %d", info->ip4_add, strlen (info->ip4_add));
      logMessage ("L2 allowed = [%s] %d", info->allowed, strlen (info->allowed));
#endif
      SHM_Lock (Lock);
      AddRec (DPtr_ip_address, &DExt_ip_address, &TaRecNum, info);
      AddKey (IPtr_ip_address_record_id, &IExt_ip_address_record_id, &TaRecNum, (TaKeyStr *) info->record_id);
      AddKey (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, &TaRecNum, (TaKeyStr *) info->ip4_add);
      AddKey (IPtr_ip_address_allowed, &IExt_ip_address_allowed, &TaRecNum, (TaKeyStr *) info->allowed);
      SHM_UnLock (Lock);
    }
}
/**************************************************************************/
void
Control ()
{
  FILE *fp = NULL;

  if ((fp = fopen (DatFName, "rb")) == NULL)
    {
      Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
      InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
      MakeFile (&DPtr_ip_address, &DExt_ip_address, DatFName, sizeof (struct Record_ip_address), (long long) 0, (long long) 0);
      MakeIndex (&IPtr_ip_address_record_id, &IExt_ip_address_record_id, IndexFName0, record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
      MakeIndex (&IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, IndexFName1, ip4_add_Len, NoDuplicates, (long long) 0, (long long) 2);
      MakeIndex (&IPtr_ip_address_allowed, &IExt_ip_address_allowed, IndexFName2, allowed_Len, Duplicates, (long long) 0, (long long) 3);
      CloseFile (DPtr_ip_address, &DExt_ip_address);
      CloseIndex (IPtr_ip_address_record_id, &IExt_ip_address_record_id);
      CloseIndex (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add);
      CloseIndex (IPtr_ip_address_allowed, &IExt_ip_address_allowed);
      TermAccess ((long long) 0);
      Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
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
