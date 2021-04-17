/**************************************************************************/
/* PROGRAM: EXAMPLE1.C                                                    */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "screen.h"
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
WINDOW *scr_save;
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
      TermEkran ();
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
  int Loading (), Inserting (), Update (), Delete (), Searching (), \
	    Find (), Next (), Prev ();
  void RebuildIndex (), Control ();

  int _choice, LastChoice, i;
  AchMatPtr Mat;
  WINDOW *scr_save;

  openLog (APP_LOGFILE);
  signal (SIGSEGV, sig_handler);
  signal (SIGINT, sig_handler);
  if (signal (SIGTERM, sig_handler) == SIG_ERR)
    logMessage ("Can't catch SIGTERM signal");

  InitEkran ();
  Lock = DB_Lock_Init (LOCK_SHM_MEM_CODE_1, LOCK_SHM_MEM_SIZE);
  SHM_Lock (Lock);
  Control (); /* Must be before InitAccess */
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);

  Frame (0, 0, LINES - 3, COLS - 1);
  Line (2, 0, 87);
  mvaddstr (1, 1, "E X A M P L E_1   P R O G R A M");
  mvaddstr (1, 33, time_and_date ());
  mvaddstr (1, 56, "HELP - CTRL-i");
  Line (LINES - 5, 0, 87);
  mvaddstr (LINES - 4, 10, "AUTHOR: VLADAN DJORDJEVIC; EMAIL:vladandjv@gmail.com");
#ifdef _M_TERMINFO
  wnoutrefresh (stdscr);
#else
  refresh ();
#endif /* _M_TERMINFO */
  for (i = 0; i < 11; i++)
    Mat[i] = (char *) malloc (18);
  strcpy (Mat[ 0], "LOADING RECORDS ");
  strcpy (Mat[ 1], "FILES RESET KEYS");
  strcpy (Mat[ 2], "INSERT RECORD   ");
  strcpy (Mat[ 3], "UPDATE RECORD   ");
  strcpy (Mat[ 4], "DELETE RECORD   ");
  strcpy (Mat[ 5], "SEARCH KEY      ");
  strcpy (Mat[ 6], "EXACT FIND KEY  ");
  strcpy (Mat[ 7], "NEXT RECORDS    ");
  strcpy (Mat[ 8], "PREV RECORDS    ");
  strcpy (Mat[ 9], "REBUILD KEYS    ");
  strcpy (Mat[10], "CREATE NEW FILES");
  info = &MyRecord;
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 2);
  OpenIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 3);
  OpenIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 4);
  OpenIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 5);
  SHM_UnLock (Lock);
  ClearKey (&IExt_client_id);
  ClearKey (&IExt_group_id);
  ClearKey (&IExt_resource_id);
  ClearKey (&IExt_auth_id);
  ClearKey (&IExt_status);
  for (;;)
    {
      _choice = Achoice ("CHOOSE:", Mat, 11, 16, 5, 2, 11, &LastChoice, T);
      scr_save = save_on_screen ();
      switch (_choice)
        {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 11:
          Delete_Screen (5, 46, 23, COLS - 2);
          Frame (18, 1, 27, COLS - 2);
        }
      switch (_choice)
        {
        case 1: _choice = Loading ();
          break;
        case 2:
          ClearKey (&IExt_client_id);
          ClearKey (&IExt_group_id);
          ClearKey (&IExt_resource_id);
          ClearKey (&IExt_auth_id);
          ClearKey (&IExt_status);
          message ("DONE  - Press any key to contiue!");
          break;
        case 3: _choice = Inserting ();
          break;
        case 4: _choice = Update ();
          break;
        case 5: _choice = Delete ();
          break;
        case 6: _choice = Searching ();
          break;
        case 7: _choice = Find ();
          break;
        case 8: _choice = Next ();
          break;
        case 9: _choice = Prev ();
          break;
        case 10:
          if (answer ("Allowed only in singluser mode! Continue? Y/N?") == F)
            break;
          SHM_Lock (Lock);
          CloseFile (DPtr, &DExt);
          CloseIndex (IPtr_client_id, &IExt_client_id);
          CloseIndex (IPtr_group_id, &IExt_group_id);
          CloseIndex (IPtr_resource_id, &IExt_resource_id);
          CloseIndex (IPtr_auth_id, &IExt_auth_id);
          CloseIndex (IPtr_status, &IExt_status);
          TermAccess ((long long) 0);
          RebuildIndex ();
          InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
          OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
          OpenIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 1);
          OpenIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 2);
          OpenIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 3);
          OpenIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 4);
          OpenIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 5);
          SHM_UnLock (Lock);
          break;
        case 11:
          if (answer ("Allowed only in singluser mode! Continue? Y/N?") == F)
            break;
          SHM_Lock (Lock);
          CloseFile (DPtr, &DExt);
          CloseIndex (IPtr_client_id, &IExt_client_id);
          CloseIndex (IPtr_group_id, &IExt_group_id);
          CloseIndex (IPtr_resource_id, &IExt_resource_id);
          CloseIndex (IPtr_auth_id, &IExt_auth_id);
          CloseIndex (IPtr_status, &IExt_status);
          TermAccess ((long long) 0);

          system ("rm -rf data1.dbc client_id.cdx group_id.cdx \
				resource_id.cdx auth_id.cd status.cdx");
          Control ();

          InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_1);
          OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
          OpenIndex (&IPtr_client_id, &IExt_client_id, IndexFName0, client_id_Len, NoDuplicates, (long long) 0, (long long) 1);
          OpenIndex (&IPtr_group_id, &IExt_group_id, IndexFName1, group_id_Len, Duplicates, (long long) 0, (long long) 2);
          OpenIndex (&IPtr_resource_id, &IExt_resource_id, IndexFName2, resource_id_Len, Duplicates, (long long) 0, (long long) 3);
          OpenIndex (&IPtr_auth_id, &IExt_auth_id, IndexFName3, auth_id_Len, Duplicates, (long long) 0, (long long) 4);
          OpenIndex (&IPtr_status, &IExt_status, IndexFName4, status_Len, Duplicates, (long long) 0, (long long) 5);
          SHM_UnLock (Lock);
          break;
        case -1:
        case -2:
          if (answer ("Do you want to leave the example program? Y/N?") == T)
            {
              delwin (scr_save);
              goto Izlaz;
            }
          continue;
        default: break;
        }
      rest_from_screen (scr_save);
      if (_choice == -2 &&
          answer ("Do you want to leave the example program? Y/N?") == T)
        break;

    }
Izlaz:
  for (i = 0; i < 11; i++)
    free (Mat[i]);
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
  TermEkran ();
  closeLog ();
  exit (0);
}
/**************************************************************************/
int
Loading ()
{
  char input[10], input1[10];
  long long z, start, end;

  empty_str (input, 10);
  empty_str (input1, 10);
  for (;;)
    {
      GetField ("BEGINNING:", 6, 21, input, 6, 9, 9, T, check_character, 0);
      GetField ("END:      ", 7, 21, input1, 7, 9, 9, T, check_character, 0);
      switch (Read ())
        {
        case -1: return (-1);
        case -2: return (-2);
        }
      start = atol (input);
      end = atol (input1);
      if (start >= end)
        {
          message ("Beginning must be less than end!");
          continue;
        }
      settext ();
#if defined( _HAS_MOUSE ) || defined( _M_TERMINFO )
      mvprintw (4, 21, "If you wish to stop it, press Esc or Ctrl-x!");
#endif
      mvprintw (19, 2, "client_id: ");
      mvprintw (20, 2, "group_id: ");
      mvprintw (21, 2, "resource_id: ");
      mvprintw (22, 2, "auth_id: ");
      mvprintw (23, 2, "socket_id: ");
      mvprintw (24, 2, "status: ");
      mvprintw (25, 2, "timestamp_l: ");
      mvprintw (26, 2, "timestamp_h: ");
      info->Deleted = 0;
      for (z = start; z <= end; z++)
        {
          uuidt (info->client_id);
#ifdef DEBUG_APP
          logMessage ("L0 client_id = [%s] %d", info->client_id, strlen (info->client_id));
#endif
          hex_string (info->group_id, 64);
          hex_string (info->resource_id, 64);
          hex_string (info->auth_id, 64);
          strcpy (info->socket_id, "127.0.0.1:4567");
          prepare_str (info->socket_id, 47);
          strcpy (info->status, "1a");
          prepare_str (info->status, 2);
          sprintf (info->timestamp_l, "%010lld", (long long) time (NULL));
          sprintf (info->timestamp_h, "%010lld", (long long) time (NULL));
#ifdef DEBUG_APP
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
          mvprintw (19, 16, "%s", info->client_id);
          mvprintw (20, 16, "%s", info->group_id);
          mvprintw (21, 16, "%s", info->resource_id);
          mvprintw (22, 16, "%s", info->auth_id);
          mvprintw (23, 16, "%s", info->socket_id);
          mvprintw (24, 16, "%s", info->status);
          mvprintw (25, 16, "%s", info->timestamp_l);
          mvprintw (26, 16, "%s", info->timestamp_h);
          refresh ();
#if defined( _HAS_MOUSE ) || defined( _M_TERMINFO )
          switch (Key_pressed ())
            {
            case 24:
            case 27:
            case 384: goto Izlaz;
            default: break;
            }
#endif
        }
Izlaz:
      refresh ();
      setgraf ();

      SHM_Lock (Lock);
      FlushFile (DPtr, &DExt);
      FlushIndex (IPtr_client_id, &IExt_client_id);
      FlushIndex (IPtr_group_id, &IExt_group_id);
      FlushIndex (IPtr_resource_id, &IExt_resource_id);
      FlushIndex (IPtr_auth_id, &IExt_auth_id);
      FlushIndex (IPtr_status, &IExt_status);
      SHM_UnLock (Lock);
      message ("DATA LOADED      Press any key to continue!");
    }
}
/**************************************************************************/
int
Inserting ()
{
  int i;

  info->Deleted = 0;
  empty_str (info->client_id, 64);
  empty_str (info->group_id, 64);
  empty_str (info->resource_id, 64);
  empty_str (info->auth_id, 64);
  empty_str (info->socket_id, 47);
  empty_str (info->status, 2);
  empty_str (info->timestamp_l, 10);
  empty_str (info->timestamp_h, 10);
  for (i = 0;;)
    {
      GetField ("client_id:   ", 4, 20, info->client_id, 4, 64, 52, T, check_character, 0);
      GetField ("group_id:    ", 5, 20, info->group_id, 5, 64, 52, T, check_character, 0);
      GetField ("resource_id: ", 6, 20, info->resource_id, 6, 64, 52, T, check_character, 0);
      GetField ("auth_id:     ", 7, 20, info->auth_id, 7, 64, 52, T, check_character, 0);
      GetField ("socket_id:   ", 8, 20, info->socket_id, 8, 46, 46, T, check_character, 0);
      GetField ("status:      ", 9, 20, info->status, 9, 2, 2, T, check_character, 0);
      GetField ("timestamp_l: ", 10, 20, info->timestamp_l, 10, 10, 10, T, check_number, 0);
      GetField ("timestamp_h: ", 11, 20, info->timestamp_h, 11, 10, 10, T, check_number, 0);
      switch (Read ())
        {
        case -1:
          if (i != 0)
            {
              SHM_Lock (Lock);
              FlushFile (DPtr, &DExt);
              FlushIndex (IPtr_client_id, &IExt_client_id);
              FlushIndex (IPtr_group_id, &IExt_group_id);
              FlushIndex (IPtr_resource_id, &IExt_resource_id);
              FlushIndex (IPtr_auth_id, &IExt_auth_id);
              FlushIndex (IPtr_status, &IExt_status);
              SHM_UnLock (Lock);
            }
          return (-1);
        case -2: return (-2);
        }
      if (answer ("Is everything correct? Y/N?") == T)
        {
          prepare_str (info->client_id, 64);
          prepare_str (info->group_id, 64);
          prepare_str (info->resource_id, 64);
          prepare_str (info->auth_id, 64);
          prepare_str (info->socket_id, 47);
          prepare_str (info->status, 2);
          prepare_str (info->timestamp_l, 10);
          prepare_str (info->timestamp_h, 10);
#ifdef DEBUG_APP
          logMessage ("\nI client_id = [%s] %d", info->client_id, strlen (info->client_id));
          logMessage ("I group_id = [%s] %d", info->group_id, strlen (info->group_id));
          logMessage ("I resource_id = [%s] %d", info->resource_id, strlen (info->resource_id));
          logMessage ("I auth_id = [%s] %d", info->auth_id, strlen (info->auth_id));
          logMessage ("I socket_id = [%s] %d", info->socket_id, strlen (info->socket_id));
          logMessage ("I status = [%s] %d", info->status, strlen (info->status));
          logMessage ("I timestamp_l = [%s] %d", info->timestamp_l, strlen (info->timestamp_l));
          logMessage ("I timestamp_h = [%s] %d", info->timestamp_h, strlen (info->timestamp_h));
#endif
          SHM_Lock (Lock);
          AddRec (DPtr, &DExt, &TaRecNum, info);
          AddKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) info->client_id);
          AddKey (IPtr_group_id, &IExt_group_id, &TaRecNum, (TaKeyStr *) info->group_id);
          AddKey (IPtr_resource_id, &IExt_resource_id, &TaRecNum, (TaKeyStr *) info->resource_id);
          AddKey (IPtr_auth_id, &IExt_auth_id, &TaRecNum, (TaKeyStr *) info->auth_id);
          AddKey (IPtr_status, &IExt_status, &TaRecNum, (TaKeyStr *) info->status);
          SHM_UnLock (Lock);
          i++;
          mvprintw (19, 2, "client_id: ");
          mvprintw (20, 2, "group_id: ");
          mvprintw (21, 2, "resource_id: ");
          mvprintw (22, 2, "auth_id: ");
          mvprintw (23, 2, "socket_id: ");
          mvprintw (24, 2, "status: ");
          mvprintw (25, 2, "timestamp_l: ");
          mvprintw (26, 2, "timestamp_h: ");
          mvprintw (19, 16, "%s", info->client_id);
          mvprintw (20, 16, "%s", info->group_id);
          mvprintw (21, 16, "%s", info->resource_id);
          mvprintw (22, 16, "%s", info->auth_id);
          mvprintw (23, 16, "%s", info->socket_id);
          mvprintw (24, 16, "%s", info->status);
          mvprintw (25, 16, "%s", info->timestamp_l);
          mvprintw (26, 16, "%s", info->timestamp_h);
          refresh ();
        }
    }
}
/**************************************************************************/
int
Update ()
{
  int i;
  char temp[65], remember[65];
  struct Record MyRecord_tmp;

  for (i = 0;;)
    {
      empty_str (info->client_id, 64);
VRH:
      Delete_Screen (5, 20, 13, COLS - 2);
      GetField ("KEY:", 6, 21, info->client_id, 7, 64, 64, T, check, check);
      switch (Read ())
        {
        case -1:
          if (i != 0)
            {
              SHM_Lock (Lock);
              FlushFile (DPtr, &DExt);
              FlushIndex (IPtr_client_id, &IExt_client_id);
              FlushIndex (IPtr_group_id, &IExt_group_id);
              FlushIndex (IPtr_resource_id, &IExt_resource_id);
              FlushIndex (IPtr_auth_id, &IExt_auth_id);
              FlushIndex (IPtr_status, &IExt_status);
              SHM_UnLock (Lock);
            }
          return (-1);
        case -2: return (-2);
        }
      strcpy (temp, info->client_id);
      prepare_str (temp, 64);
      strcpy (remember, temp);
      SHM_Lock (Lock);
      FindKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          mvprintw (19, 2, "client_id: ");
          mvprintw (20, 2, "group_id: ");
          mvprintw (21, 2, "resource_id: ");
          mvprintw (22, 2, "auth_id: ");
          mvprintw (23, 2, "socket_id: ");
          mvprintw (24, 2, "status: ");
          mvprintw (25, 2, "timestamp_l: ");
          mvprintw (26, 2, "timestamp_h: ");
          GetRec (DPtr, &DExt, TaRecNum, info);
          SHM_UnLock (Lock);
          for (;;)
            {
              mvprintw (19, 16, "%s", info->client_id);
              mvprintw (20, 16, "%s", info->group_id);
              mvprintw (21, 16, "%s", info->resource_id);
              mvprintw (22, 16, "%s", info->auth_id);
              mvprintw (23, 16, "%s", info->socket_id);
              mvprintw (24, 16, "%s", info->status);
              mvprintw (25, 16, "%s", info->timestamp_l);
              mvprintw (26, 16, "%s", info->timestamp_h);
              refresh ();
              if (answer ("Continue with the same key? Y/N?") == F)
                break;
              SHM_Lock (Lock);
              NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              if (OKAY == T && strcmp (temp, remember) == 0)
                {
                  GetRec (DPtr, &DExt, TaRecNum, info);
                  SHM_UnLock (Lock);
                }
              else
                {
                  SHM_UnLock (Lock);
                  message ("No more records with the same key!");
                  break;
                }
              mvprintw (19, 16, "%s", info->client_id);
              mvprintw (20, 16, "%s", info->group_id);
              mvprintw (21, 16, "%s", info->resource_id);
              mvprintw (22, 16, "%s", info->auth_id);
              mvprintw (23, 16, "%s", info->socket_id);
              mvprintw (24, 16, "%s", info->status);
              mvprintw (25, 16, "%s", info->timestamp_l);
              mvprintw (26, 16, "%s", info->timestamp_h);
              refresh ();
            }
          MyRecord_tmp = MyRecord;
          for (;;)
            {
              GetField ("group_id:    ", 5, 20, info->group_id, 5, 64, 52, T, check_character, 0);
              GetField ("resource_id: ", 6, 20, info->resource_id, 6, 64, 52, T, check_character, 0);
              GetField ("auth_id:     ", 7, 20, info->auth_id, 7, 64, 52, T, check_character, 0);
              GetField ("socket_id:   ", 8, 20, info->socket_id, 8, 46, 46, T, check_character, 0);
              GetField ("status:      ", 9, 20, info->status, 9, 2, 2, T, check_character, 0);
              GetField ("timestamp_l: ", 10, 20, info->timestamp_l, 10, 10, 10, T, check_number, 0);
              GetField ("timestamp_h: ", 11, 20, info->timestamp_h, 11, 10, 10, T, check_number, 0);

              switch (Read ())
                {
                case -1: goto VRH;
                case -2: return (-2);
                }
              if (answer ("Is everything correct? Y/N?") == T)
                {
                  prepare_str (info->group_id, 64);
                  prepare_str (info->resource_id, 64);
                  prepare_str (info->auth_id, 64);
                  prepare_str (info->socket_id, 47);
                  prepare_str (info->status, 2);
                  prepare_str (info->timestamp_l, 10);
                  prepare_str (info->timestamp_h, 10);
                  strcpy (temp, info->client_id);
                  SHM_Lock (Lock);
                  FindKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
                  if (OKAY == T)
                    {
                      PutRec (DPtr, &DExt, TaRecNum, info);
                      DeleteKey (IPtr_group_id, &IExt_group_id, &TaRecNum, (TaKeyStr *) MyRecord_tmp.group_id);
                      AddKey (IPtr_group_id, &IExt_group_id, &TaRecNum, (TaKeyStr *) info->group_id);
                      DeleteKey (IPtr_resource_id, &IExt_resource_id, &TaRecNum, (TaKeyStr *) MyRecord_tmp.resource_id);
                      AddKey (IPtr_resource_id, &IExt_resource_id, &TaRecNum, (TaKeyStr *) info->resource_id);
                      DeleteKey (IPtr_auth_id, &IExt_auth_id, &TaRecNum, (TaKeyStr *) MyRecord_tmp.auth_id);
                      AddKey (IPtr_auth_id, &IExt_auth_id, &TaRecNum, (TaKeyStr *) info->auth_id);
                      DeleteKey (IPtr_status, &IExt_status, &TaRecNum, (TaKeyStr *) MyRecord_tmp.status);
                      AddKey (IPtr_status, &IExt_status, &TaRecNum, (TaKeyStr *) info->status);
                      SHM_UnLock (Lock);
                      i++;
                    }
                  else
                    {
                      SHM_UnLock (Lock);
                      message ("The record is deleted meanwhile!");
                    }
                  break;
                }
            }
        }
      else
        {
          SHM_Lock (Lock);
          message ("There is no requested key!");
        }
    }
}
/**************************************************************************/
int
Delete ()
{
  int i;
  char input[65], temp[65], remember[65];
  struct Record *info1;
  struct Record MyRecord1;

  info1 = &MyRecord1;
  for (i = 0;;)
    {
      empty_str (input, 64);
      GetField ("KEY    :", 6, 21, input, 6, 10, 10, T, check, check);
      switch (Read ())
        {
        case -1:
          if (i != 0)
            {
              SHM_Lock (Lock);
              FlushFile (DPtr, &DExt);
              FlushIndex (IPtr_client_id, &IExt_client_id);
              FlushIndex (IPtr_group_id, &IExt_group_id);
              FlushIndex (IPtr_resource_id, &IExt_resource_id);
              FlushIndex (IPtr_auth_id, &IExt_auth_id);
              FlushIndex (IPtr_status, &IExt_status);
              SHM_UnLock (Lock);
            }
          return (-1);
        case -2: return (-2);
        }
      strcpy (temp, input);
      prepare_str (temp, 64);
      strcpy (remember, temp);
      SHM_Lock (Lock);
      FindKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          mvprintw (19, 2, "client_id: ");
          mvprintw (20, 2, "group_id: ");
          mvprintw (21, 2, "resource_id: ");
          mvprintw (22, 2, "auth_id: ");
          mvprintw (23, 2, "socket_id: ");
          mvprintw (24, 2, "status: ");
          mvprintw (25, 2, "timestamp_l: ");
          mvprintw (26, 2, "timestamp_h: ");
          GetRec (DPtr, &DExt, TaRecNum, info);
          SHM_UnLock (Lock);
          for (;;)
            {
              mvprintw (19, 16, "%s", info->client_id);
              mvprintw (20, 16, "%s", info->group_id);
              mvprintw (21, 16, "%s", info->resource_id);
              mvprintw (22, 16, "%s", info->auth_id);
              mvprintw (23, 16, "%s", info->socket_id);
              mvprintw (24, 16, "%s", info->status);
              mvprintw (25, 16, "%s", info->timestamp_l);
              mvprintw (26, 16, "%s", info->timestamp_h);
              refresh ();
              if (answer ("Continue with the same key? Y/N?") == F)
                break;
              SHM_Lock (Lock);
              NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              if (OKAY == T && strcmp (temp, remember) == 0)
                {
                  GetRec (DPtr, &DExt, TaRecNum, info);
                  SHM_UnLock (Lock);
                }
              else
                {
                  SHM_UnLock (Lock);
                  message ("No more records with the same key!");
                  break;
                }
            }
          strcpy (temp, info->client_id);
          strcpy (info1->client_id, info->client_id);
          /*strcpy(info1->group_id, info->group_id);
          strcpy(info1->resource_id, info->resource_id);
          strcpy(info1->socket_id, info->auth_id);
          strcpy(info1->status, info->status); */
          if (answer ("Delete? Y/N?") == T)
            {
              strcpy (temp, info->client_id);
              SHM_Lock (Lock);
              FindKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              if (OKAY == T)
                {
                  GetRec (DPtr, &DExt, TaRecNum, info);
                  if (strcmp (info1->client_id, info->client_id) == 0)
                    {
                      DeleteRec (DPtr, &DExt, TaRecNum);
                      DeleteKey (IPtr_client_id, &IExt_client_id, &TaRecNum, temp);
                      DeleteKey (IPtr_group_id, &IExt_group_id, &TaRecNum, (TaKeyStr *) MyRecord1.group_id);
                      DeleteKey (IPtr_resource_id, &IExt_resource_id, &TaRecNum, (TaKeyStr *) MyRecord1.resource_id);
                      DeleteKey (IPtr_auth_id, &IExt_auth_id, &TaRecNum, (TaKeyStr *) MyRecord1.auth_id);
                      DeleteKey (IPtr_status, &IExt_status, &TaRecNum, (TaKeyStr *) MyRecord1.status);
                      i++;
                    }
                }
              SHM_UnLock (Lock);
            }
        }
      else
        {
          SHM_Lock (Lock);
          message ("There is no requested key!");
        }
    }
}
/**************************************************************************/
int
Searching ()
{
  int sl;
  char input[65], temp[65], remember[65];

  empty_str (input, 64);
  for (;;)
    {
      GetField ("KEY    :", 6, 21, input, 6, 64, 64, T, check_character, 0);
      switch (Read ())
        {
        case -1: return (-1);
        case -2: return (-2);
        }
      strcpy (temp, input);
      prepare_str (temp, 64);
      strcpy (remember, temp);
      trim_str (remember);
      sl = strlen (remember);
      SHM_Lock (Lock);
      SearchKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T && strcmp (remember, left (temp, sl)) == 0)
        {
          GetRec (DPtr, &DExt, TaRecNum, info);
          SHM_UnLock (Lock);
          mvprintw (19, 2, "client_id: ");
          mvprintw (20, 2, "group_id: ");
          mvprintw (21, 2, "resource_id: ");
          mvprintw (22, 2, "auth_id: ");
          mvprintw (23, 2, "socket_id: ");
          mvprintw (24, 2, "status: ");
          mvprintw (25, 2, "timestamp_l: ");
          mvprintw (26, 2, "timestamp_h: ");
          for (;;)
            {
              mvprintw (19, 16, "%s", info->client_id);
              mvprintw (20, 16, "%s", info->group_id);
              mvprintw (21, 16, "%s", info->resource_id);
              mvprintw (22, 16, "%s", info->auth_id);
              mvprintw (23, 16, "%s", info->socket_id);
              mvprintw (24, 16, "%s", info->status);
              mvprintw (25, 16, "%s", info->timestamp_l);
              mvprintw (26, 16, "%s", info->timestamp_h);
              refresh ();
              if (answer ("Continue with the same root of key? Y/N?") == F)
                break;
              SHM_Lock (Lock);
              NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              if (OKAY == T && strcmp (remember, left (temp, sl)) == 0)
                {
                  GetRec (DPtr, &DExt, TaRecNum, info);
                  SHM_UnLock (Lock);
                }
              else
                {
                  SHM_UnLock (Lock);
                  message ("No more records with the same root of key!");
                  break;
                }
            }
          strcpy (input, info->client_id);
        }
      else
        {
          SHM_UnLock (Lock);
          message ("There is no requested root of key!");
        }
    }
}
/**************************************************************************/
int
Find ()
{
  char input[65], temp[65], remember[65];

  for (;;)
    {
      empty_str (input, 64);
      GetField ("KEY:", 6, 21, input, 7, 64, 64, T, check, check);
      switch (Read ())
        {
        case -1: return (-1);
        case -2: return (-2);
        }
      strcpy (temp, input);
      prepare_str (temp, 64);
      strcpy (remember, temp);
      SHM_Lock (Lock);
      FindKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          GetRec (DPtr, &DExt, TaRecNum, info);
          SHM_UnLock (Lock);
          mvprintw (19, 2, "client_id: ");
          mvprintw (20, 2, "group_id: ");
          mvprintw (21, 2, "resource_id: ");
          mvprintw (22, 2, "auth_id: ");
          mvprintw (23, 2, "socket_id: ");
          mvprintw (24, 2, "status: ");
          mvprintw (25, 2, "timestamp_l: ");
          mvprintw (26, 2, "timestamp_h: ");
          for (;;)
            {
              mvprintw (19, 16, "%s", info->client_id);
              mvprintw (20, 16, "%s", info->group_id);
              mvprintw (21, 16, "%s", info->resource_id);
              mvprintw (22, 16, "%s", info->auth_id);
              mvprintw (23, 16, "%s", info->socket_id);
              mvprintw (24, 16, "%s", info->status);
              mvprintw (25, 16, "%s", info->timestamp_l);
              mvprintw (26, 16, "%s", info->timestamp_h);
              refresh ();
              if (answer ("Continue with the same key? Y/N?") == F)
                break;
              SHM_Lock (Lock);
              NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              if (OKAY == T && strcmp (temp, remember) == 0)
                {
                  SHM_UnLock (Lock);
                  GetRec (DPtr, &DExt, TaRecNum, info);
                }
              else
                {
                  SHM_UnLock (Lock);
                  message ("No more records with the same key!");
                  break;
                }
            }
        }
      else
        {
          SHM_UnLock (Lock);
          message ("There is no requested key!");
        }
    }
}
/**************************************************************************/
int
Next ()
{
  char temp[70];
  int control, i, j, l, m, OrdNum, err, watch, remember;
  char Mat[LINES - 9][340];
  extern WINDOW *Winf;

  extern int HowMuchW, StartPositionW;
  extern long int ReadPositionW;

  HowMuchW = 0;
  StartPositionW = 0;
  ReadPositionW = 0;
  remember = 0;
  j = 0;
  Delete_Screen (5, 1, LINES - 9, COLS - 2);
  Line (4, 0, 87);
  for (;;)
    {
#ifdef _M_TERMCAP
      refresh ();
      settext ();
#endif
      mvaddstr (LINES - 2, 6, "PgDn - Ctrl-f; PgUp - Ctrl-b; CTRL-X FOR EXIT");
#ifdef _M_TERMCAP
      refresh ();
      setgraf ();
#endif
      strcpy (Mat[0], " NUM.                             CLIENT_ID                                                        GROUP_ID                                                      RESOURCE_ID                                                       AUTH_ID                                             SOCKET_ID                          ST  TIMEST_L  TIMEST_H  ");
#ifdef DEBUG_APP
      logMessage ("Mat[0]=[%s] %d", Mat[0], strlen (Mat[0]));
#endif
AGAIN:
      control = 0;
      watch = 0;
      OrdNum = 1;
WORKDOWN:
      if (control == 2)
        {
          OrdNum += (LINES - 9);
          for (; remember != 2; remember--)
            {
              SHM_Lock (Lock);
              NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              SHM_UnLock (Lock);
              if (OKAY == F)
                break;
            }
        }
      control = 1;
      if (watch != 3 || j == LINES - 9)
        watch = 0;
      err = 0;
      for (i = 1, j = 1, remember = 1; j < LINES - 9; i++, OrdNum++, j++, remember++)
        {
          SHM_Lock (Lock);
          NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
          if (OKAY == T)
            {
              GetRec (DPtr, &DExt, TaRecNum, info);
              SHM_UnLock (Lock);
              sprintf (Mat[j], "%5.0d  ", OrdNum);
#ifdef DEBUG_APP
              logMessage ("\nN client_id = [%s] %d", info->client_id, strlen (info->client_id));
              logMessage ("N group_id = [%s] %d", info->group_id, strlen (info->group_id));
              logMessage ("N resource_id = [%s] %d", info->resource_id, strlen (info->resource_id));
              logMessage ("N auth_id = [%s] %d", info->auth_id, strlen (info->auth_id));
              logMessage ("N socket_id = [%s] %d", info->socket_id, strlen (info->socket_id));
              logMessage ("N status = [%s] %d", info->status, strlen (info->status));
              logMessage ("N timestamp_l = [%s] %d", info->timestamp_l, strlen (info->timestamp_l));
              logMessage ("N timestamp_h = [%s] %d", info->timestamp_h, strlen (info->timestamp_h));
#endif
              strcat (Mat[j], info->client_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->group_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->resource_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->auth_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->socket_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->status);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_l);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_h);
#ifdef DEBUG_APP
              logMessage ("M Mat[%d] = [%s] %d", j, Mat[j], strlen (Mat[j]));
#endif
            }
          else
            {
              SHM_UnLock (Lock);
              err = 1;
              OrdNum--;
              if (j == 1)
                {
                  if (watch == 3)
                    {
                      wattron (Winf, A_BLINK);
                      message ("We have already worn you that you can't go down!");
                      wattroff (Winf, A_BLINK);
                    }
                  else
                    {
                      if (OrdNum == 0)
                        {
                          Delete_Screen (5, 1, LINES - 4, COLS - 2);
                          mvaddstr ((int) LINES / 2, (int) COLS / 3 + 3, "THERE ARE NO RECORDS!");
                          refresh ();
                          message ("Press any key to contiue!");
                          return (0);
                        }
                      else
                        message ("Bottom! You can't go down!");
                    }
                  watch = 3;
                  control = 1;
                  remember = 0;
                  OrdNum += (LINES - 9);
                  goto WORKUP;
                }
              break;
            }
        }
FURTHER:
      GetSquare (3, 1, Mat[0], 340, 77);
      if (control == 0 || control == 1)
        {
          for (l = 1, m = 5; l < j; l++, m++)
            GetSquare (m, 1, Mat[l], 340, 77);
          l += 4;
          Delete_Screen (l, 1, LINES - 4, COLS - 2);
        }
      else
        {
          for (l = 1, m = 5; l < LINES - 9; l++, m++)
            GetSquare (m, 1, Mat[l], 340, 77);
        }
      switch (Look ())
        {
        case -1: return (-1);
        case -2: return (-2);
        case -3:
          if (watch == 2)
            {
              wattron (Winf, A_BLINK);
              message ("We have already worn you that you can't go up!");
              wattroff (Winf, A_BLINK);
              goto FURTHER;
            }
          if (err == 2 || (OrdNum == LINES - 9 && control == 1 && remember >= (LINES - 9)) || (OrdNum >= 0 && OrdNum < LINES - 10))
            {
              message ("Top! You can't go up!");
              watch = 2;
              goto FURTHER;
            }
          goto WORKUP;
        case -4:
          if (watch == 1)
            {
              wattron (Winf, A_BLINK);
              message ("We have already worn you that you can't go down!");
              wattroff (Winf, A_BLINK);
              goto FURTHER;
            }
          if (err == 1)
            {
              message ("Bottom! You can't go down!");
              watch = 1;
              goto FURTHER;
            }
          goto WORKDOWN;
        }
WORKUP:
      if (control == 1)
        {
          if (j == 1)
            {
              OrdNum -= (LINES - 9);
              for (; remember != 0; remember--)
                {
                  PrevKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
                  if (OKAY == F)
                    break;
                }
            }
          if (j < (LINES - 9))
            {
              for (; j != 1; j--)
                OrdNum--;
              i--;
              for (; i != 0; i--)
                {
                  PrevKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
                  if (OKAY == F)
                    break;
                }
            }
          else
            {
              OrdNum -= (LINES - 9);
              for (; i != 2; i--)
                {
                  PrevKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
                  if (OKAY == F)
                    break;
                }
            }
        }
      control = 2;
      if (watch != 3)
        watch = 0;
      err = 0;
      for (i = LINES - 10, j = LINES - 10, remember = 1; j > 0; i--, OrdNum--, j--, remember++)
        {
          SHM_Lock (Lock);
          PrevKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
          SHM_UnLock (Lock);
          if (OKAY == T)
            {
              SHM_Lock (Lock);
              GetRec (DPtr, &DExt, TaRecNum, info);
              SHM_UnLock (Lock);
#ifdef DEBUG_APP
              logMessage ("\nNN client_id = [%s] %d", info->client_id, strlen (info->client_id));
              logMessage ("NN group_id = [%s] %d", info->group_id, strlen (info->group_id));
              logMessage ("NN resource_id = [%s] %d", info->resource_id, strlen (info->resource_id));
              logMessage ("NN auth_id = [%s] %d", info->auth_id, strlen (info->auth_id));
              logMessage ("NN socket_id = [%s] %d", info->socket_id, strlen (info->socket_id));
              logMessage ("NN status = [%s] %d", info->status, strlen (info->status));
              logMessage ("NN timestamp_l = [%s] %d", info->timestamp_l, strlen (info->timestamp_l));
              logMessage ("NN timestamp_h = [%s] %d", info->timestamp_h, strlen (info->timestamp_h));
#endif
              sprintf (Mat[j], "%5.0d  ", OrdNum);
              strcat (Mat[j], info->client_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->group_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->resource_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->auth_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->socket_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->status);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_l);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_h);
#ifdef DEBUG_APP
              logMessage ("MM Mat[%d] = [%s] %d", j, Mat[j], strlen (Mat[j]));
#endif
            }
          else
            {
              ClearKey (&IExt_client_id);
              goto AGAIN;
            }
        }
      goto FURTHER;
    }
}
/**************************************************************************/
int
Prev ()
{
  char temp[70];
  int control, i, j, l, m, OrdNum, err, watch, remember;
  char Mat[LINES - 9][340];
  extern WINDOW *Winf;

  extern int HowMuchW, StartPositionW;
  extern long int ReadPositionW;

  HowMuchW = 0;
  StartPositionW = 0;
  ReadPositionW = 0;
  remember = 0;
  j = 0;
  Delete_Screen (5, 1, LINES - 9, COLS - 2);
  Line (4, 0, 87);
  for (;;)
    {
#ifdef _M_TERMCAP
      refresh ();
      settext ();
#endif
      mvaddstr (LINES - 2, 6, "PgDn - Ctrl-f; PgUp - Ctrl-b; CTRL-X FOR EXIT");
#ifdef _M_TERMCAP
      refresh ();
      setgraf ();
#endif
      strcpy (Mat[0], " NUM.                             CLIENT_ID                                                        GROUP_ID                                                      RESOURCE_ID                                                       AUTH_ID                                             SOCKET_ID                          ST  TIMEST_L  TIMEST_H  ");
AGAIN:
      control = 0;
      watch = 0;
      OrdNum = 1;
WORKDOWN:
      if (control == 2)
        {
          OrdNum += (LINES - 9);
          for (; remember != 2; remember--)
            {
              PrevKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              if (OKAY == F)
                break;
            }
        }
      control = 1;
      if (watch != 3 || j == LINES - 7)
        watch = 0;
      err = 0;
      for (i = 1, j = 1, remember = 1; j < LINES - 9; i++, OrdNum++, j++, remember++)
        {
          SHM_Lock (Lock);
          PrevKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
          SHM_UnLock (Lock);
          if (OKAY == T)
            {
              SHM_Lock (Lock);
              GetRec (DPtr, &DExt, TaRecNum, info);
              SHM_UnLock (Lock);
              sprintf (Mat[j], "%5.0d  ", OrdNum);
              strcat (Mat[j], info->client_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->group_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->resource_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->auth_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->socket_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->status);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_l);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_h);
            }
          else
            {
              err = 1;
              OrdNum--;
              if (j == 1)
                {
                  if (watch == 3)
                    {
                      wattron (Winf, A_BLINK);
                      message ("We have already worn you that you can't go down!");
                      wattroff (Winf, A_BLINK);
                    }
                  else
                    {
                      if (OrdNum == 0)
                        {
                          Delete_Screen (5, 1, LINES - 4, COLS - 2);
                          mvaddstr ((int) LINES / 2, (int) COLS / 3 + 3, "THERE ARE NO RECORDS!");
                          refresh ();
                          message ("Press any key to contiue!");
                          return (0);
                        }
                      else
                        message ("Bottom! You can't go down!");
                    }
                  watch = 3;
                  control = 1;
                  remember = 0;
                  OrdNum += (LINES - 9);
                  goto WORKUP;
                }
              break;
            }
        }
FURTHER:
      GetSquare (3, 1, Mat[0], 340, 77);
      if (control == 0 || control == 1)
        {
          for (l = 1, m = 5; l < j; l++, m++)
            GetSquare (m, 1, Mat[l], 340, 77);
          l += 4;
          Delete_Screen (l, 1, LINES - 4, COLS - 2);
        }
      else
        {
          for (l = 1, m = 5; l < LINES - 9; l++, m++)
            GetSquare (m, 1, Mat[l], 340, 77);
        }
      switch (Look ())
        {
        case -1: return (-1);
        case -2: return (-2);
        case -3:
          if (watch == 2)
            {
              wattron (Winf, A_BLINK);
              message ("We have already worn you that you can't go up!");
              wattroff (Winf, A_BLINK);
              goto FURTHER;
            }
          if (err == 2 || (OrdNum == LINES - 9 && control == 1 && remember >= (LINES - 9)) || (OrdNum >= 0 && OrdNum < LINES - 10))
            {
              message ("Top! You can't go up!");
              watch = 2;
              goto FURTHER;
            }
          goto WORKUP;
        case -4:
          if (watch == 1)
            {
              wattron (Winf, A_BLINK);
              message ("We have already worn you that you can't go down!");
              wattroff (Winf, A_BLINK);
              goto FURTHER;
            }
          if (err == 1)
            {
              message ("Bottom! You can't go down!");
              watch = 1;
              goto FURTHER;
            }
          goto WORKDOWN;
        }
WORKUP:
      if (control == 1)
        {
          if (j == 1)
            {
              OrdNum -= (LINES - 9);
              for (; remember != 0; remember--)
                {
                  NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
                  if (OKAY == F)
                    break;
                }
            }
          if (j < (LINES - 9))
            {
              for (; j != 1; j--)
                OrdNum--;
              i--;
              for (; i != 0; i--)
                {
                  NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
                  if (OKAY == F)
                    break;
                }
            }
          else
            {
              OrdNum -= (LINES - 9);
              for (; i != 2; i--)
                {
                  NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
                  if (OKAY == F)
                    break;
                }
            }
        }
      control = 2;
      if (watch != 3)
        watch = 0;
      err = 0;
      for (i = LINES - 10, j = LINES - 10, remember = 1; j > 0; i--, OrdNum--, j--, remember++)
        {
          SHM_Lock (Lock);
          NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
          if (OKAY == T)
            {
              GetRec (DPtr, &DExt, TaRecNum, info);
              SHM_UnLock (Lock);
              sprintf (Mat[j], "%5.0d  ", OrdNum);
              strcat (Mat[j], info->client_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->group_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->resource_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->auth_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->socket_id);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->status);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_l);
              strcat (Mat[j], " ");
              strcat (Mat[j], info->timestamp_h);
            }
          else
            {
              SHM_UnLock (Lock);
              ClearKey (&IExt_client_id);
              goto AGAIN;
            }
        }
      goto FURTHER;
    }
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
  mvaddstr (6, 21, "PACKING");
  mvprintw (7, 21, "Number of records:  %10ld", velic);
  mvprintw (14, 21, "Packed: ");
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
      mvprintw (14, 41, "%10ld", RecordNumber);
      refresh ();
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
  mvaddstr (6, 21, "INDEXING");
  mvprintw (7, 21, "Number of records:  %10ld", velic);
  mvprintw (14, 21, "Indexed: ");
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
      mvprintw (14, 41, "%10ld", RecordNumber);
      refresh ();
    }
  CloseIndex (IPtr_client_id, &IExt_client_id);
  CloseIndex (IPtr_group_id, &IExt_group_id);
  CloseIndex (IPtr_resource_id, &IExt_resource_id);
  CloseIndex (IPtr_auth_id, &IExt_auth_id);
  CloseIndex (IPtr_status, &IExt_status);
  CloseFile (DPtr, &DExt);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_1, (size_t) MACCESS_SHM_MEM_SIZE);
  message ("DONE!    Press any key to continue.");
}
/**************************************************************************/
void
Control ()
{
  FILE *fp = NULL;

  if ((fp = fopen (DatFName, "rb")) == NULL)
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
/**************************************************************************/
void
Help (char *Headline)
{
  savescreen ();
  ShowSymbols ();
  restscreen ();
}
/****************************** END ***************************************/
