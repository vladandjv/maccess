/**************************************************************************/
/* PROGRAM: EXAMPLE.C                                                     */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "time_date.h"
#include "screen.h"
#include "example.h"

FileName DatFName = "data.dbc";
FileName IndexFName = "data.cdx";
FileName DatFName1 = "data.tmp";
unsigned long long KeyLen = 11; /* Should be key lenght + 1 */
struct IndexExt IExt;
struct DataExt DExt;
struct Record OurRecord;
struct Record *info = NULL;
DataFilePtr DPtr = NULL;
DataFilePtr IPtr = NULL;
long long TaRecNum = 0;
WINDOW *scr_save = NULL;

void sig_handler(int sig)
{

#ifdef DEBUG_APP
  logMessage("Cought signal %d in sigint_handler", sig);
#endif

  switch (sig)
  {
  case SIGTERM:
  case SIGSEGV:
    SHM_Lock();
    CloseFile(DPtr, &DExt);
    CloseIndex(IPtr, &IExt);
    SHM_UnLock();
    TermAccess();
    SHM_UnLock();
    TermEkran();
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
int main(void)
{
  int Loading(), Inserting(), Update(), Delete(), Deleting(), Searching(), Find();
  int Next(), Prev();
  void RebuildIndex(), Control();
  char out[22];

  long long _choice, LastChoice, i;
  AchMatPtr Mat;
  WINDOW *scr_save;

  openLog(APP_LOGFILE);
  signal(SIGSEGV, sig_handler);
  signal(SIGINT, sig_handler);
  if (signal(SIGTERM, sig_handler) == SIG_ERR)
    logMessage("Can't catch SIGTERM signal");

  InitEkran();
#ifndef SINGLE_USER_NO_SHARED_MEMORY
  DB_Lock_Init(SEMAPHORE_CODE);
#endif
  SHM_Lock();
  Control(); /* Must be before InitAccess */
  InitAccess(MACCESS_SHM_MEM_CODE);

  Frame(0, 0, LINES - 3, COLS - 1);
  Line(2, 0, 87);
  mvaddstr(1, 1, "E X A M P L E   P R O G R A M");
  mvaddstr(1, 33, time_and_date(out));
  mvaddstr(1, 56, "HELP - CTRL-i");
  refresh();
  Line(LINES - 5, 0, 87);
  mvaddstr(LINES - 4, 10, "AUTHOR: VLADAN DJORDJEVIC; EMAIL:vladandjv@gmail.com");
  wnoutrefresh(stdscr);
  for (i = 0; i < 12; i++)
    Mat[i] = (char *)malloc(17);
  strcpy(Mat[0], "LOADING RECORDS ");
  strcpy(Mat[1], "FILES RESET KEYS");
  strcpy(Mat[2], "INSERT RECORD   ");
  strcpy(Mat[3], "UPDATE RECORD   ");
  strcpy(Mat[4], "DELETE RECORD   ");
  strcpy(Mat[5], "MULTI DELETE    ");
  strcpy(Mat[6], "SEARCH KEY      ");
  strcpy(Mat[7], "EXACT FIND KEY  ");
  strcpy(Mat[8], "NEXT RECORDS    ");
  strcpy(Mat[9], "PREVIOUS RECORDS");
  strcpy(Mat[10], "REBUILD KEYS    ");
  strcpy(Mat[11], "CREATE NEW FILES");
  info = &OurRecord;
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  SHM_UnLock();
  ClearKey(&IExt);
  for (;;)
  {
    _choice = Achoice("CHOOSE:", Mat, 12, 16, 5, 2, 12, &LastChoice, T);
    scr_save = save_on_screen();
    switch (_choice)
    {
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 12:
      Delete_Screen(5, 46, 17, 76);
      Frame(5, 20, 15, 76);
      Line(10, 20, 87);
    }
    switch (_choice)
    {
    case 1:
      _choice = Loading();
      break;
    case 2:
      ClearKey(&IExt);
      message("DONE  - Press any key to contiue!");
      break;
    case 3:
      _choice = Inserting();
      break;
    case 4:
      _choice = Update();
      break;
    case 5:
      _choice = Delete();
      break;
    case 6:
      _choice = Deleting();
      break;
    case 7:
      _choice = Searching();
      break;
    case 8:
      _choice = Find();
      break;
    case 9:
      _choice = Next();
      break;
    case 10:
      _choice = Prev();
      break;
    case 11:
      if (answer("Allowed only in singluser mode! Continue? Y/N?") == F)
        break;
      SHM_Lock();
      CloseFile(DPtr, &DExt);
      CloseIndex(IPtr, &IExt);
      TermAccess();
      RebuildIndex();
      InitAccess(MACCESS_SHM_MEM_CODE);
      OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
      OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
      SHM_UnLock();
      break;
    case 12:
      if (answer("Allowed only in singluser mode! Continue? Y/N?") == F)
        break;
      SHM_Lock();
      CloseFile(DPtr, &DExt);
      CloseIndex(IPtr, &IExt);
      TermAccess();

      if (system("rm -rf data.dbc data.cdx") == -1)
      {
        message("System call error! Can't delete file.");
        TermEkran();
        exit(0);
      }
      Control();

      InitAccess(MACCESS_SHM_MEM_CODE);
      OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
      OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
      SHM_UnLock();
      break;
    case -1:
    case -2:
      if (answer("Do you want to leave the example program? Y/N?") == T)
      {
        delwin(scr_save);
        goto Exit;
      }
      continue;
    default:
      break;
    }
    rest_from_screen(scr_save);
    if (_choice == -2 &&
        answer("Do you want to leave the example program? Y/N?") == T)
      break;
  }
Exit:
  for (i = 0; i < 13; i++)
    free(Mat[i]);
  SHM_Lock();
  CloseFile(DPtr, &DExt);
  CloseIndex(IPtr, &IExt);
  SHM_UnLock();
  TermAccess();
  TermEkran();
  closeLog();
  exit(0);
}
/**************************************************************************/
int Loading()
{
  char input[10], input1[10], beginning[22], duration[22], completion[22], out[22];
  long long z, start, end;

  strcpy(input, empty_string(out, 9));
  strcpy(input1, empty_string(out, 9));
  for (;;)
  {
    GetField("BEGINNING:", 6, 21, input, 6, 9, 9, T, check_character, 0);
    GetField("END:      ", 7, 21, input1, 7, 9, 9, T, check_character, 0);
    switch (Read())
    {
    case -1:
      return (-1);
    case -2:
      return (-2);
    }
    start = atol(input);
    end = atol(input1);
    if (start >= end)
    {
      message("Beginning must be less than end!");
      continue;
    }
    mvprintw(4, 21, "If you wish to stop it, press Esc, Ctrl-x or f8!");
    mvprintw(11, 21, "Key: ");
    mvprintw(12, 21, "Surname: ");
    mvprintw(13, 21, "Name: ");
    mvprintw(14, 21, "Remark: ");
    strcpy(beginning, time_and_date(out));
    mvprintw(16, 21, "TIME: Beginning: %s", beginning);
    mvprintw(17, 27, "Lasting:");
    info->Deleted = 0;
    for (z = start; z <= end; z++)
    {
      sprintf(info->Key, "%010lld", z);
      strcpy(info->Key, to_left(out, info->Key, 10));
      sprintf(info->Surname, "%025lld", z);
      sprintf(info->Name, "%020lld", z);
      sprintf(info->Remark, "%045lld", z);
      SHM_Lock();
      AddRec(DPtr, &DExt, &TaRecNum, info);
      AddKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)info->Key);
      SHM_UnLock();
      mvprintw(11, 30, "%s", info->Key);
      mvprintw(12, 30, "%s", info->Surname);
      mvprintw(13, 30, "%s", info->Name);
      mvprintw(14, 30, "%s", info->Remark);
      strcpy(completion, time_and_date(out));
      strcpy(duration, time_difference(out, beginning, completion));
      strcpy(duration, vrmuredi(out, duration));
      mvprintw(17, 38, "%s", duration);
      refresh();
      switch (Key_pressed())
      {
      case 24:
      case 27:
      case 384:
        goto Exit;
      default:
        break;
      }
    }
  Exit:
    mvprintw(18, 27, "End:       %s", completion);
    refresh();
    SHM_Lock();
    FlushFile(DPtr, &DExt);
    FlushIndex(IPtr, &IExt);
    SHM_UnLock();
    message("DATA LOADED      Press any key to continue!");
  }
}
/**************************************************************************/
int Inserting()
{
  int i;
  char input[11], out[11];

  info->Deleted = 0;
  strcpy(input, empty_string(out, 10));
  strcpy(info->Surname, empty_string(out, 25));
  strcpy(info->Name, empty_string(out, 20));
  strcpy(info->Remark, empty_string(out, 45));
  for (i = 0;;)
  {
    GetField("KEY    :", 6, 21, input, 6, 10, 10, T, check_character, 0);
    GetField("SURNAME:", 7, 21, info->Surname, 7, 25, 25, T, check_character, 0);
    GetField("NAME:   ", 8, 21, info->Name, 8, 20, 20, T, check_character, 0);
    GetField("REMARK: ", 9, 21, info->Remark, 9, 45, 45, T, 0, 0);
    switch (Read())
    {
    case -1:
      if (i != 0)
      {
        SHM_Lock();
        FlushFile(DPtr, &DExt);
        FlushIndex(IPtr, &IExt);
        SHM_UnLock();
      }
      return (-1);
    case -2:
      return (-2);
    }
    if (answer("Is everything correct? Y/N?") == T)
    {
      strcpy(info->Key, to_left(out, input, 10));
      SHM_Lock();
      AddRec(DPtr, &DExt, &TaRecNum, info);
      AddKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)info->Key);
      SHM_UnLock();
      i++;
      mvprintw(11, 21, "SURNAME: %s", info->Surname);
      mvprintw(12, 21, "NAME:    %s", info->Name);
      mvprintw(13, 21, "REMARK:  %s", info->Remark);
      refresh();
    }
  }
}
/**************************************************************************/
int Update()
{
  long long i;
  char temp[11], remember[11], out[11];

  strcpy(info->Key, empty_string(out, 10));
  for (i = 0;;)
  {
  VRH:
    GetField("KEY:    ", 6, 21, info->Key, 6, 10, 10, T, check, check);
    switch (Read())
    {
    case -1:
      if (i != 0)
        FlushFile(DPtr, &DExt);
      return (-1);
    case -2:
      return (-2);
    }
    strcpy(temp, info->Key);
    strcpy(remember, temp);
    SHM_Lock();
    FindKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
    SHM_UnLock();
    if (OKAY == T)
    {
      GetRec(DPtr, &DExt, TaRecNum, info);
      SHM_UnLock();
      mvprintw(11, 21, "SURNAME:");
      mvprintw(12, 21, "NAME:");
      mvprintw(13, 21, "REMARK:");
      for (;;)
      {
        mvprintw(11, 30, "%s", info->Surname);
        mvprintw(12, 30, "%s", info->Name);
        mvprintw(13, 30, "%s", info->Remark);
        refresh();
        if (answer("Continue with the same key? Y/N?") == F)
          break;
        SHM_Lock();
        NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
        if (OKAY == T && strcmp(temp, remember) == 0)
        {
          GetRec(DPtr, &DExt, TaRecNum, info);
          SHM_UnLock();
        }
        else
        {
          SHM_UnLock();
          message("No more records with the same key!");
          break;
        }
        mvprintw(11, 30, "%s", info->Surname);
        mvprintw(12, 30, "%s", info->Name);
        mvprintw(13, 30, "%s", info->Remark);
        refresh();
      }
      for (;;)
      {
        GetField("SURNAME:", 7, 21, info->Surname, 7, 25, 25, T, check_character, 0);
        GetField("NAME:   ", 8, 21, info->Name, 8, 20, 20, T, check_character, 0);
        GetField("REMARK: ", 9, 21, info->Remark, 9, 45, 45, T, 0, 0);
        switch (Read())
        {
        case -1:
          goto VRH;
        case -2:
          return (-2);
        }
        if (answer("Is everything correct? Y/N?") == T)
        {
          strcpy(temp, info->Key);
          SHM_Lock();
          FindKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
          if (OKAY == T)
          {
            PutRec(DPtr, &DExt, TaRecNum, info);
            SHM_UnLock();
            i++;
          }
          else
          {
            SHM_UnLock();
            message("The record is deleted meanwhile!");
          }
          break;
        }
      }
    }
    else
    {
      SHM_UnLock();
      message("There is no requested key!");
    }
  }
}
/**************************************************************************/
int Delete()
{
  long long i;
  char input[11], temp[11], remember[11], out[11];
  struct Record *info1;
  struct Record OurRecord1;

  info1 = &OurRecord1;
  strcpy(input, empty_string(out, 10));
  for (i = 0;;)
  {
    GetField("KEY    :", 6, 21, input, 6, 10, 10, T, check, check);
    switch (Read())
    {
    case -1:
      if (i != 0)
      {
        FlushFile(DPtr, &DExt);
        FlushIndex(IPtr, &IExt);
      }
      return (-1);
    case -2:
      return (-2);
    }
    strcpy(temp, to_left(out, input, 10));
    strcpy(remember, temp);
    SHM_Lock();
    FindKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
    if (OKAY == T)
    {
      GetRec(DPtr, &DExt, TaRecNum, info);
      SHM_UnLock();
      for (;;)
      {
        mvprintw(11, 21, "SURNAME: %s", info->Surname);
        mvprintw(12, 21, "NAME:    %s", info->Name);
        mvprintw(13, 21, "REMARK:  %s", info->Remark);
        refresh();
        if (answer("Continue with the same key? Y/N?") == F)
          break;
        SHM_Lock();
        NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
        if (OKAY == T && strcmp(temp, remember) == 0)
        {
          GetRec(DPtr, &DExt, TaRecNum, info);
          SHM_UnLock();
        }
        else
        {
          SHM_UnLock();
          message("No more records with the same key!");
          break;
        }
      }
      strcpy(temp, info->Key);
      strcpy(info1->Key, info->Key);
      strcpy(info1->Surname, info->Surname);
      strcpy(info1->Name, info->Name);
      if (answer("Delete? Y/N?") == T)
      {
        SHM_Lock();
        FindKey(IPtr, &IExt, &TaRecNum, temp);
        if (OKAY == T)
        {
          GetRec(DPtr, &DExt, TaRecNum, info);
          if (strcmp(info1->Key, info->Key) == 0 && strcmp(info1->Surname, info->Surname) == 0 && strcmp(info1->Name, info->Name) == 0)
          {
            DeleteRec(DPtr, &DExt, TaRecNum);
            DeleteKey(IPtr, &IExt, &TaRecNum, temp);
            i++;
          }
        }
        SHM_UnLock();
      }
    }
    else
    {
      SHM_Lock();
      message("There is no requested key!");
    }
  }
}
/**************************************************************************/
int Deleting()
{
  char input[10], input1[10], temp[11], out[11];
  long long z, start, end, i;

  strcpy(input, empty_string(out, 9));
  strcpy(input1, empty_string(out, 9));
  for (;;)
  {
    GetField("BEGINNING:", 6, 21, input, 6, 9, 9, T, check_character, 0);
    GetField("END:      ", 7, 21, input1, 7, 9, 9, T, check_character, 0);
    switch (Read())
    {
    case -1:
      return (-1);
    case -2:
      return (-2);
    }
    start = atol(input);
    end = atol(input1);
    if (start >= end)
    {
      message("Beginning must be less than end!");
      continue;
    }
    Delete_Screen(11, 21, 14, 75);
    mvprintw(11, 21, "SEARCHED KEY:");
    mvprintw(12, 21, "DELETED KEY:");
    mvprintw(14, 21, "TOTAL NUMBER OF DELETED RECORDS: 0");
    for (z = start, i = 0; z <= end; z++)
    {
      sprintf(temp, "%010lld", z);
      mvprintw(11, 35, "%s", temp);
      SHM_Lock();
      FindKey(IPtr, &IExt, &TaRecNum, temp);
      if (OKAY == T)
      {
        DeleteRec(DPtr, &DExt, TaRecNum);
        DeleteKey(IPtr, &IExt, &TaRecNum, temp);
        mvprintw(12, 35, "%s", temp);
        i++;
        mvprintw(14, 54, "%ld", i);
      }
      SHM_UnLock();
      refresh();
    }
    SHM_Lock();
    FlushFile(DPtr, &DExt);
    FlushIndex(IPtr, &IExt);
    SHM_UnLock();
    message("DATA DELETED     Press any key to continue!");
  }
}
/**************************************************************************/
int Searching()
{
  long long sl;
  char input[11], temp[11], remember[11], out[11];

  strcpy(input, empty_string(out, 10));
  for (;;)
  {
    GetField("KEY    :", 6, 21, input, 6, 10, 10, T, check_character, 0);
    switch (Read())
    {
    case -1:
      return (-1);
    case -2:
      return (-2);
    }
    Delete_Screen(11, 21, 14, 75);
    strcpy(temp, to_left(out, input, 10));
    strcpy(remember, temp);
    strcpy(remember, trim(out, remember));
    sl = strlen(remember);
    SHM_Lock();
    SearchKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
    if (OKAY == T && strcmp(remember, left(out, temp, sl)) == 0)
    {
      GetRec(DPtr, &DExt, TaRecNum, info);
      SHM_UnLock();
      mvprintw(11, 21, "KEY:");
      mvprintw(12, 21, "SURNAME:");
      mvprintw(13, 21, "NAME:");
      mvprintw(14, 21, "REMARK:");
      for (;;)
      {
        mvprintw(11, 31, "%s", info->Key);
        mvprintw(12, 31, "%s", info->Surname);
        mvprintw(13, 31, "%s", info->Name);
        mvprintw(14, 31, "%s", info->Remark);
        refresh();
        if (answer("Continue with the same root of key? Y/N?") == F)
          break;
        SHM_Lock();
        NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
        if (OKAY == T && strcmp(remember, left(out, temp, sl)) == 0)
        {
          GetRec(DPtr, &DExt, TaRecNum, info);
          SHM_UnLock();
        }
        else
        {
          SHM_UnLock();
          message("No more records with the same root of key!");
          break;
        }
      }
      strcpy(input, info->Key);
    }
    else
    {
      SHM_UnLock();
      message("There is no requested root of key!");
    }
  }
}
/**************************************************************************/
int Find()
{
  char input[11], temp[11], remember[11], out[11];

  strcpy(input, empty_string(out, 10));
  for (;;)
  {
    GetField("KEY    :", 6, 21, input, 6, 10, 10, T, check, check);
    switch (Read())
    {
    case -1:
      return (-1);
    case -2:
      return (-2);
    }
    Delete_Screen(11, 21, 14, 75);
    strcpy(temp, to_left(out, input, 10));
    strcpy(remember, temp);
    SHM_Lock();
    FindKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
    SHM_UnLock();
    if (OKAY == T)
    {
      GetRec(DPtr, &DExt, TaRecNum, info);
      SHM_UnLock();
      mvprintw(11, 21, "KEY:");
      mvprintw(12, 21, "SURNAME:");
      mvprintw(13, 21, "NAME:");
      mvprintw(14, 21, "REMARK:");
      for (;;)
      {
        mvprintw(11, 31, "%s", info->Key);
        mvprintw(12, 31, "%s", info->Surname);
        mvprintw(13, 31, "%s", info->Name);
        mvprintw(14, 31, "%s", info->Remark);
        refresh();
        if (answer("Continue with the same key? Y/N?") == F)
          break;
        SHM_Lock();
        NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
        if (OKAY == T && strcmp(temp, remember) == 0)
        {
          SHM_Lock();
          GetRec(DPtr, &DExt, TaRecNum, info);
        }
        else
        {
          SHM_Lock();
          message("No more records with the same key!");
          break;
        }
      }
    }
    else
    {
      SHM_UnLock();
      message("There is no requested key!");
    }
  }
}
/**************************************************************************/
int Next()
{
  char temp[50], out[50];
  long long control, i, j, l, m, OrdNum, err, watch, remember;
  char Mat[LINES - 7][115];
  extern WINDOW *Winf;

  extern long long HowMuchW, StartPositionW;
  extern long long ReadPositionW;

  HowMuchW = 0;
  StartPositionW = 0;
  ReadPositionW = 0;
  remember = 0;
  j = 0;
  Delete_Screen(5, 1, LINES - 4, 78);
  Line(4, 0, 87);
  for (;;)
  {
    mvaddstr(LINES - 2, 6, "PgDn - Ctrl-f; PgUp - Ctrl-b; CTRL-X FOR EXIT");
    strcpy(Mat[0], " RBR.     KEY         SURNAME                      NAME                         REMARK                        ");
  AGAIN:
    control = 0;
    watch = 0;
    OrdNum = 1;
  WORKDOWN:
    if (control == 2)
    {
      OrdNum += (LINES - 7);
      for (; remember != 2; remember--)
      {
        SHM_Lock();
        NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
        SHM_UnLock();
        if (OKAY == F)
          break;
      }
    }
    control = 1;
    if (watch != 3 || j == LINES - 7)
      watch = 0;
    err = 0;
    for (i = 1, j = 1, remember = 1; j < LINES - 7; i++, OrdNum++, j++, remember++)
    {
      SHM_Lock();
      NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
      if (OKAY == T)
      {
        GetRec(DPtr, &DExt, TaRecNum, info);
        SHM_UnLock();
        sprintf(Mat[j], "%5.0lld  ", OrdNum);
        strcat(Mat[j], info->Key);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Surname);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Name);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Remark);
      }
      else
      {
        SHM_UnLock();
        err = 1;
        OrdNum--;
        if (j == 1)
        {
          if (watch == 3)
          {
            wattron(Winf, A_BLINK);
            message("We have already worn you that you can't go down!");
            wattroff(Winf, A_BLINK);
          }
          else
          {
            if (OrdNum == 0)
            {
              Delete_Screen(5, 1, LINES - 4, 78);
              mvaddstr((long long)LINES / 2, (long long)COLS / 3 + 3, "THERE ARE NO RECORDS!");
              refresh();
              message("Press any key to contiue!");
              return (0);
            }
            else
              message("Bottom! You can't go down!");
          }
          watch = 3;
          control = 1;
          remember = 0;
          OrdNum += (LINES - 7);
          goto WORKUP;
        }
        break;
      }
    }
  FURTHER:
    GetSquare(3, 1, Mat[0], 110, 77);
    if (control == 0 || control == 1)
    {
      for (l = 1, m = 5; l < j; l++, m++)
        GetSquare(m, 1, Mat[l], 110, 77);
      l += 4;
      Delete_Screen(l, 1, LINES - 4, 78);
    }
    else
    {
      for (l = 1, m = 5; l < LINES - 7; l++, m++)
        GetSquare(m, 1, Mat[l], 110, 77);
    }
    switch (Look())
    {
    case -1:
      return (-1);
    case -2:
      return (-2);
    case -3:
      if (watch == 2)
      {
        wattron(Winf, A_BLINK);
        message("We have already worn you that you can't go up!");
        wattroff(Winf, A_BLINK);
        goto FURTHER;
      }
      if (err == 2 || (OrdNum == LINES - 7 && control == 1 && remember >= (LINES - 7)) || (OrdNum >= 0 && OrdNum < LINES - 8))
      {
        message("Top! You can't go up!");
        watch = 2;
        goto FURTHER;
      }
      goto WORKUP;
    case -4:
      if (watch == 1)
      {
        wattron(Winf, A_BLINK);
        message("We have already worn you that you can't go down!");
        wattroff(Winf, A_BLINK);
        goto FURTHER;
      }
      if (err == 1)
      {
        message("Bottom! You can't go down!");
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
        OrdNum -= (LINES - 7);
        for (; remember != 0; remember--)
        {
          PrevKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
          if (OKAY == F)
            break;
        }
      }
      if (j < (LINES - 7))
      {
        for (; j != 1; j--)
          OrdNum--;
        i--;
        for (; i != 0; i--)
        {
          PrevKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
          if (OKAY == F)
            break;
        }
      }
      else
      {
        OrdNum -= (LINES - 7);
        for (; i != 2; i--)
        {
          PrevKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
          if (OKAY == F)
            break;
        }
      }
    }
    control = 2;
    if (watch != 3)
      watch = 0;
    err = 0;
    for (i = LINES - 8, j = LINES - 8, remember = 1; j > 0; i--, OrdNum--, j--, remember++)
    {
      SHM_Lock();
      PrevKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
      if (OKAY == T)
      {
        GetRec(DPtr, &DExt, TaRecNum, info);
        SHM_UnLock();
        sprintf(Mat[j], "%5.0lld  ", OrdNum);
        strcat(Mat[j], info->Key);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Surname);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Name);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Remark);
      }
      else
      {
        SHM_UnLock();
        ClearKey(&IExt);
        goto AGAIN;
      }
    }
    goto FURTHER;
  }
}
/**************************************************************************/
int Prev()
{
  char temp[50], out[50];
  long long control, i, j, l, m, OrdNum, err, watch, remember;
  char Mat[LINES - 7][115];
  extern WINDOW *Winf;

  extern long long HowMuchW, StartPositionW;
  extern long long ReadPositionW;

  HowMuchW = 0;
  StartPositionW = 0;
  ReadPositionW = 0;
  remember = 0;
  j = 0;
  Delete_Screen(5, 1, LINES - 4, 78);
  Line(4, 0, 87);
  for (;;)
  {
    mvaddstr(LINES - 2, 6, "PgDn - Ctrl-f; PgUp - Ctrl-b; CTRL-X FOR EXIT");
    strcpy(Mat[0], " RBR.     KEY         SURNAME                      NAME                         REMARK                        ");
  AGAIN:
    control = 0;
    watch = 0;
    OrdNum = 1;
  WORKDOWN:
    if (control == 2)
    {
      OrdNum += (LINES - 7);
      for (; remember != 2; remember--)
      {
        SHM_Lock();
        PrevKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
        SHM_UnLock();
        if (OKAY == F)
          break;
      }
    }
    control = 1;
    if (watch != 3 || j == LINES - 7)
      watch = 0;
    err = 0;
    for (i = 1, j = 1, remember = 1; j < LINES - 7; i++, OrdNum++, j++, remember++)
    {
      SHM_Lock();
      PrevKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
      if (OKAY == T)
      {
        GetRec(DPtr, &DExt, TaRecNum, info);
        SHM_UnLock();
        sprintf(Mat[j], "%5.0lld  ", OrdNum);
        strcat(Mat[j], info->Key);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Surname);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Name);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Remark);
      }
      else
      {
        SHM_UnLock();
        err = 1;
        OrdNum--;
        if (j == 1)
        {
          if (watch == 3)
          {
            wattron(Winf, A_BLINK);
            message("We have already worn you that you can't go down!");
            wattroff(Winf, A_BLINK);
          }
          else
          {
            if (OrdNum == 0)
            {
              Delete_Screen(5, 1, LINES - 4, 78);
              mvaddstr((long long)LINES / 2, (long long)COLS / 3 + 3, "THERE ARE NO RECORDS!");
              refresh();
              message("Press any key to contiue!");
              return (0);
            }
            else
              message("Bottom! You can't go down!");
          }
          watch = 3;
          control = 1;
          remember = 0;
          OrdNum += (LINES - 7);
          goto WORKUP;
        }
        break;
      }
    }
  FURTHER:
    GetSquare(3, 1, Mat[0], 110, 77); /* Because terminfo 78-1=77 or without frame. */
    if (control == 0 || control == 1)
    {
      for (l = 1, m = 5; l < j; l++, m++)
        GetSquare(m, 1, Mat[l], 110, 77);
      l += 4;
      Delete_Screen(l, 1, LINES - 4, 78);
    }
    else
    {
      for (l = 1, m = 5; l < LINES - 7; l++, m++)
        GetSquare(m, 1, Mat[l], 110, 77);
    }
    switch (Look())
    {
    case -1:
      return (-1);
    case -2:
      return (-2);
    case -3:
      if (watch == 2)
      {
        wattron(Winf, A_BLINK);
        message("We have already worn you that you can't go up!");
        wattroff(Winf, A_BLINK);
        goto FURTHER;
      }
      if (err == 2 || (OrdNum == LINES - 7 && control == 1 && remember >= (LINES - 7)) || (OrdNum >= 0 && OrdNum < LINES - 8))
      {
        message("Top! You can't go up!");
        watch = 2;
        goto FURTHER;
      }
      goto WORKUP;
    case -4:
      if (watch == 1)
      {
        wattron(Winf, A_BLINK);
        message("We have already worn you that you can't go down!");
        wattroff(Winf, A_BLINK);
        goto FURTHER;
      }
      if (err == 1)
      {
        message("Bottom! You can't go down!");
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
        OrdNum -= (LINES - 7);
        for (; remember != 0; remember--)
        {
          NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
          if (OKAY == F)
            break;
        }
      }
      if (j < (LINES - 7))
      {
        for (; j != 1; j--)
          OrdNum--;
        i--;
        for (; i != 0; i--)
        {
          NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
          if (OKAY == F)
            break;
        }
      }
      else
      {
        OrdNum -= (LINES - 7);
        for (; i != 2; i--)
        {
          NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
          if (OKAY == F)
            break;
        }
      }
    }
    control = 2;
    if (watch != 3)
      watch = 0;
    err = 0;
    for (i = LINES - 8, j = LINES - 8, remember = 1; j > 0; i--, OrdNum--, j--, remember++)
    {
      SHM_Lock();
      NextKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)temp);
      if (OKAY == T)
      {
        GetRec(DPtr, &DExt, TaRecNum, info);
        SHM_UnLock();
        sprintf(Mat[j], "%5.0lld  ", OrdNum);
        strcat(Mat[j], info->Key);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Surname);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Name);
        strcat(Mat[j], empty_string(out, 1));
        strcat(Mat[j], info->Remark);
      }
      else
      {
        SHM_UnLock();
        ClearKey(&IExt);
        goto AGAIN;
      }
    }
    goto FURTHER;
  }
}
/**************************************************************************/
void RebuildIndex()
{
  long long RecordNumber, RecNum, velic;
  struct Record CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
  InitAccess(MACCESS_SHM_MEM_CODE);
  MakeFile(&DPtr1, &DExt1, DatFName1, sizeof(struct Record), (long long)0);
  CloseFile(DPtr1, &DExt1);
  TermAccess();
  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);

  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr1, &DExt1, DatFName1, sizeof(struct Record), (long long)0);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)1);
  velic = FileLen(DPtr, &DExt) - 1;
  mvaddstr(6, 21, "PACKING");
  mvprintw(7, 21, "Number of records:  %10ld", velic);
  mvprintw(14, 21, "Packed: ");
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
  {
    GetRec(DPtr, &DExt, RecordNumber, &CurRec);
    if (CurRec.Deleted == 0)
    {
      CurRec1.Deleted = CurRec.Deleted;
      strcpy(CurRec1.Key, CurRec.Key);
      strcpy(CurRec1.Surname, CurRec.Surname);
      strcpy(CurRec1.Name, CurRec.Name);
      strcpy(CurRec1.Remark, CurRec.Remark);
      AddRec(DPtr1, &DExt1, &RecNum, &CurRec1);
    }
    mvprintw(14, 41, "%10ld", RecordNumber);
    refresh();
  }
  CloseFile(DPtr, &DExt);
  CloseFile(DPtr1, &DExt1);
  TermAccess();
  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);

  if (system("mv data.tmp data.dbc; rm -rf data.cdx") == -1)
  {
    message("System call error! Can't delete file.");
    TermEkran();
    exit(0);
  }

  InitAccess(MACCESS_SHM_MEM_CODE);
  MakeIndex(&IPtr, &IExt, IndexFName, KeyLen, NoDuplicates, (long long)0);
  CloseIndex(IPtr, &IExt);
  TermAccess();
  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);

  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  velic = FileLen(DPtr, &DExt) - 1;
  mvaddstr(6, 21, "INDEXING");
  mvprintw(7, 21, "Number of records:  %10ld", velic);
  mvprintw(14, 21, "Indexed: ");
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
  {
    GetRec(DPtr, &DExt, RecordNumber, &CurRec);
    if (CurRec.Deleted == 0)
      AddKey(IPtr, &IExt, &RecordNumber, (TaKeyStr *)CurRec.Key);
    mvprintw(14, 41, "%10ld", RecordNumber);
    refresh();
  }
  CloseIndex(IPtr, &IExt);
  CloseFile(DPtr, &DExt);
  TermAccess();
  Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
  message("DONE!    Press any key to continue.");
}
/**************************************************************************/
void Control()
{
  FILE *fp = NULL;

  if ((fp = fopen(DatFName, "rb")) == NULL)
  {
    Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
    InitAccess(MACCESS_SHM_MEM_CODE);
    MakeFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
    MakeIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
    CloseFile(DPtr, &DExt);
    CloseIndex(IPtr, &IExt);
    TermAccess();
    Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE);
  }
  else
    fclose(fp);
}
/**************************************************************************/
void
    Help(Headline)

        char *Headline;
{
  savescreen();
  ShowSymbols();
  restscreen();
}
/****************************** END ***************************************/
