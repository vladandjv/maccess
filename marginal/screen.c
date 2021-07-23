//**************************************************************************/
/* PROGRAM: SCREEN.C                                                      */
/* DESCRIPTION:                                                           */
/*    Low-level rutines for working with screens                          */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "defs.h"
#include "screen.h"
/************************** constants *************************************/
#define MaxGet 60
#define MaxLook LINES - 7 /* Must be less or equal MaxGet-u. */
#define MaxGetLen MAX_STRING_LENGTH
#define MaxHeadlineLen 80

Boolean Direct_Exit = F;
WINDOW *Wsav = NULL;
/****************************** types *************************************/
typedef char GetStr[MaxGetLen];
typedef char HeadlineStr[MaxHeadlineLen];

struct TakeField
{
  GetStr *FieldPtr;
  HeadlineStr Headline;
  long long RowN;
  long long KolN;
  GetStr InputField;
  long long RowUp;
  long long KolG;
  long long VelPolja;
  long long RowLength;
  long long ZadevKey_pressed;
  Boolean Azuriran;
  Boolean FirstevKey_pressed;
  Boolean (*FunPtr)();
  Boolean (*ListPtr)();
  Boolean Datum;
};
/*************************  variables *************************************/
long long InsertMod;
long long XIns;
long long YIns;

WINDOW *Winf;
struct TakeField *InFieldPtr[MaxGet];

long long StartPositionW, HowMuchW;
long long ReadPositionW;
Boolean Trajan;

WINDOW *Wkur;
/************************* FUNCTIONS OF EDITOR ****************************/
void InitEkran()
{
  long long i;

  for (i = 0; i < MaxGet; i++)
    InFieldPtr[i] = (struct TakeField *)0;

  initscr();
  crmode();
  noecho();
  raw();
  nonl();
  Wsav = newwin(0, 0, 0, 0);
  Wkur = newwin(1, 1, LINES - 1, COLS - 1);
  Winf = newwin(1, COLS - 1, LINES - 1, 0);
  XIns = (COLS - 6);
  YIns = 0;
  InsertMod = 0;

  if (start_color() == OK)
  {
    init_pair(PAIR1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PAIR2, COLOR_WHITE, COLOR_BLUE);
    init_pair(PAIR3, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR4, COLOR_CYAN, COLOR_RED);
    init_pair(PAIR5, COLOR_CYAN, COLOR_BLACK);
    init_pair(PAIR6, COLOR_WHITE, COLOR_BLACK);
  }
  wattron(stdscr, COLOR_PAIR(PAIR1));
  wattron(Winf, COLOR_PAIR(PAIR5));
}
/***************************************************************************/
void TermEkran()
{
  wclear(stdscr);
  wrefresh(stdscr);
  endwin();
}
/***************************************************************************/
long long
    Achoice(Headline, Matrica, Rows, Sirina, wUp, wLeft, Elements, LastChoice, SlovoI)

        char *Headline;
AchMatPtr Matrica;
long long Rows, Sirina, wUp, wLeft, Elements, *LastChoice;
Boolean SlovoI;
{
  void wCrtica();
  long long Taken, i, Choice, ChoiceRow, FirsRow;
  WINDOW *Wach;
  char out[MasAchRow];

  Choice = 1;
  FirsRow = 0;
  ChoiceRow = 1;

  Rows = min(Rows, Elements);
  wUp--;
  wLeft--;

  mvaddstr(wUp - 1, wLeft, Headline);
  wnoutrefresh(stdscr);
  Wach = newwin(Rows + 2, Sirina + 2, wUp, wLeft);
  wattron(Wach, COLOR_PAIR(PAIR2));
  wKutija(Wach, Rows + 2, Sirina + 2, F);
  if (*LastChoice > 0 && *LastChoice <= Elements)
  {
    for (; Choice != *LastChoice;)
    {
      Choice++;
      if (ChoiceRow == Rows)
        FirsRow++;
      else
        ChoiceRow++;
    }
    for (i = 0; i < Rows; i++)
      mvwaddstr(Wach, i + 1, 1, left(out, Matrica[FirsRow + i], Sirina));
  }
  else
  {
    for (i = 2; i <= Rows; i++)
      mvwaddstr(Wach, i, 1, left(out, Matrica[i - 1], Sirina));
  }
  if (FirsRow != 0)
    mvwprintw(Wach, 1, Sirina + 1, "%c", '^');
  if (FirsRow != Elements - Rows)
    mvwprintw(Wach, Rows, Sirina + 1, "%c", 'V');
  wattrset(Wach, A_STANDOUT | COLOR_PAIR(PAIR6));
  if (*LastChoice > 0 && *LastChoice <= Elements)
    mvwaddstr(Wach, ChoiceRow, 1, left(out, Matrica[Choice - 1], Sirina));
  else
    mvwaddstr(Wach, 1, 1, left(out, Matrica[0], Sirina));
  wattroff(Wach, A_STANDOUT | COLOR_PAIR(PAIR6));
  wmove(Wach, Rows + 1, Sirina + 1);
  wnoutrefresh(Wach);
  doupdate();

  wattron(Wach, COLOR_PAIR(PAIR2));
  for (;;)
  {
    Taken = evKey_pressed();
    switch (Taken)
    {
    case 366: /* V */
      if (Choice == Elements)
      {
        Choice = 1;
        FirsRow = 0;
        ChoiceRow = 1;
      }
      else
      {
        Choice++;
        if (ChoiceRow == Rows)
          FirsRow++;
        else
          ChoiceRow++;
      }
      if (FirsRow != 0)
        mvwprintw(Wach, 1, Sirina + 1, "%c", '^');
      else
        wCrtica(Wach, 1, Sirina + 1);
      if (FirsRow != Elements - Rows)
        mvwprintw(Wach, Rows, Sirina + 1, "%c", 'V');
      else
        wCrtica(Wach, Rows, Sirina + 1);
      break;
    case 365: /* ^ */
      if (Choice == 1)
      {
        Choice = Elements;
        FirsRow = Elements - Rows;
        ChoiceRow = Rows;
      }
      else
      {
        Choice--;
        if (ChoiceRow == 1)
          FirsRow--;
        else
          ChoiceRow--;
      }
      if (FirsRow != 0)
        mvwprintw(Wach, 1, Sirina + 1, "%c", '^');
      else
        wCrtica(Wach, 1, Sirina + 1);
      if (FirsRow != Elements - Rows)
        mvwprintw(Wach, Rows, Sirina + 1, "%c", 'V');
      else
        wCrtica(Wach, Rows, Sirina + 1);
      break;
    case 1: /* Ctrl-a */
      Choice = 1;
      FirsRow = 0;
      ChoiceRow = 1;
      if (FirsRow != Elements - Rows)
        mvwprintw(Wach, Rows, Sirina + 1, "%c", 'V');
      else
        wCrtica(Wach, Rows, Sirina + 1);
      wCrtica(Wach, 1, Sirina + 1);
      break;
    case 5: /* Ctrl-e */
      Choice = Elements;
      FirsRow = Elements - Rows;
      ChoiceRow = Rows;
      if (FirsRow != 0)
        mvwprintw(Wach, 1, Sirina + 1, "%c", '^');
      else
        wCrtica(Wach, 1, Sirina + 1);
      wCrtica(Wach, Rows, Sirina + 1);
      break;
    case 6: /* Ctrl-f*/
      if (Choice + Rows <= Elements && Elements - (Choice + Rows) >= Rows)
      {
        Choice += Rows;
        FirsRow += Rows;
      }
      else
      {
        Choice = Elements;
        FirsRow = Elements - Rows;
        ChoiceRow = Rows;
      }
      if (FirsRow != 0)
        mvwprintw(Wach, 1, Sirina + 1, "%c", '^');
      else
        wCrtica(Wach, 1, Sirina + 1);
      if (FirsRow != Elements - Rows)
        mvwprintw(Wach, Rows, Sirina + 1, "%c", 'V');
      else
        wCrtica(Wach, Rows, Sirina + 1);
      break;
    case 2: /* Ctrl-b */
      if (Choice - Rows > 0 && (Choice - Rows) >= Rows)
      {
        Choice -= Rows;
        FirsRow -= Rows;
      }
      else
      {
        Choice = 1;
        FirsRow = 0;
        ChoiceRow = 1;
      }
      if (FirsRow != 0)
        mvwprintw(Wach, 1, Sirina + 1, "%c", '^');
      else
        wCrtica(Wach, 1, Sirina + 1);
      if (FirsRow != Elements - Rows)
        mvwprintw(Wach, Rows, Sirina + 1, "%c", 'V');
      else
        wCrtica(Wach, Rows, Sirina + 1);
      break;
    case 13:  /* Enter */
    case 367: /* ----> */
      *LastChoice = Choice;
      wattroff(stdscr, COLOR_PAIR(PAIR1));
      overwrite(Wach, stdscr);
      wattron(stdscr, COLOR_PAIR(PAIR1));
      delwin(Wach);
      return (Choice);
      /* Interruption: */
    case 27:  /* Esc */
    case 24:  /* Ctrl-x */
    case 384: /* f8 */
    case 368: /* <---- */
      *LastChoice = Choice;
      delwin(Wach);
      return (-1);
      /* Help: */
    case 9: /* Ctrl-i */
      wattroff(stdscr, COLOR_PAIR(PAIR1));
      overwrite(Wach, stdscr);
      Help("ACHOICE");
      wattron(stdscr, COLOR_PAIR(PAIR1));
      break;
    case 382: /* f6 */
    case 11:  /* Ctrl-k */
      *LastChoice = Choice;
      delwin(Wach);
      return (-2);
    default:
      if (Taken > 31 && Taken < 127) /* letters and numbers */
      {
        Taken = toupper(Taken);
        for (i = Choice; i <= Elements; i++)
        {
          if (Matrica[i - 1][0] == Taken)
          {
            Choice = i;
            if (SlovoI == T)
              goto Izlaz;
          }
        }
        if (Choice != i)
        {
          for (i = 1; i < Choice; i++)
          {
            if (Matrica[i - 1][0] == Taken)
            {
              Choice = i;
              if (SlovoI == T)
                goto Izlaz;
            }
          }
        }
        if (Elements - Choice <= Rows)
        {
          ChoiceRow = Rows - Elements + Choice;
          FirsRow = Choice - ChoiceRow;
        }
        else
        {
          ChoiceRow = 1;
          FirsRow = Choice - 1;
        }
      }
      break;
    }
    for (i = 0; i < Rows; i++)
      mvwaddstr(Wach, i + 1, 1, left(out, Matrica[FirsRow + i], Sirina));
    wattron(Wach, A_STANDOUT | COLOR_PAIR(PAIR6));
    mvwaddstr(Wach, ChoiceRow, 1, left(out, Matrica[Choice - 1], Sirina));
    wattroff(Wach, A_STANDOUT | COLOR_PAIR(PAIR6));
    wmove(Wach, Rows + 1, Sirina + 1);
    wrefresh(Wach);
    wattron(Wach, COLOR_PAIR(PAIR2));
  }
Izlaz:
  if (Elements - Choice <= Rows)
  {
    ChoiceRow = Rows - Elements + Choice;
    FirsRow = Choice - ChoiceRow;
  }
  else
  {
    ChoiceRow = 1;
    FirsRow = Choice - 1;
  }
  for (i = 0; i < Rows; i++)
    mvwaddstr(Wach, i + 1, 1, left(out, Matrica[FirsRow + i], Sirina));
  wattron(Wach, A_STANDOUT | COLOR_PAIR(PAIR6));
  mvwaddstr(Wach, ChoiceRow, 1, left(out, Matrica[Choice - 1], Sirina));
  wattroff(Wach, A_STANDOUT | COLOR_PAIR(PAIR6));
  wmove(Wach, Rows + 1, Sirina + 1);
  wnoutrefresh(Wach);
  *LastChoice = Choice;
  wattroff(stdscr, COLOR_PAIR(PAIR1));
  overwrite(Wach, stdscr);
  wattron(stdscr, COLOR_PAIR(PAIR1));
  delwin(Wach);
  return (Choice);
}
/***************************************************************************/
void
    wCrtica(MyWindow, wUp, wLeft)

        WINDOW *MyWindow;
long long wUp, wLeft;
{
  wattron(MyWindow, A_ALTCHARSET);
  mvwprintw(MyWindow, wUp, wLeft, "%c", ACS_VLINE);
  wattroff(MyWindow, A_ALTCHARSET);
  wnoutrefresh(MyWindow);
}
/***************************************************************************/
void
    wKutija(MyWindow, Rows, Sirina, Delete_Screen)

        WINDOW *MyWindow;
long long Rows, Sirina;
Boolean Delete_Screen;
{
  if (Delete_Screen == T)
    wDelete_Screen(MyWindow, 1, 1, Rows - 2, Sirina - 2);
  wFrame(MyWindow, 0, 0, Rows - 1, Sirina - 1);
}
/***************************************************************************/
void GetField(Headline, RowN, KolN, InputField, RowUp, VelPolja, RowLength, FirstevKey_pressed, FPtr, LPtr)

    void *Headline;
long long RowN, KolN;
void *InputField;
long long RowUp, VelPolja, RowLength;
Boolean FirstevKey_pressed;
Boolean (*FPtr)();
Boolean (*LPtr)();
{
  long long i;

  for (i = 0; i < MaxGet; i++)
  {
    if (InFieldPtr[i] == (struct TakeField *)0)
    {
      InFieldPtr[i] = (struct TakeField *)
          malloc(sizeof(struct TakeField));
      (*InFieldPtr[i]).FieldPtr = (GetStr *)InputField;
      break;
    }
  }
  strcpy((*InFieldPtr[i]).Headline, (char *)Headline);
  (*InFieldPtr[i]).RowN = RowN;
  (*InFieldPtr[i]).KolN = KolN;
  strcpy((*InFieldPtr[i]).InputField, (char *)InputField);
  (*InFieldPtr[i]).RowUp = RowUp;
  (*InFieldPtr[i]).VelPolja = VelPolja;
  (*InFieldPtr[i]).RowLength = RowLength;
  (*InFieldPtr[i]).ZadevKey_pressed = 0;
  (*InFieldPtr[i]).Azuriran = F;
  (*InFieldPtr[i]).FirstevKey_pressed = FirstevKey_pressed;
  (*InFieldPtr[i]).FunPtr = FPtr;
  (*InFieldPtr[i]).ListPtr = LPtr;
  (*InFieldPtr[i]).Datum = F;
}
/***************************************************************************/
void GetDatum(Headline, RowN, KolN, InputField, RowUp, FirstevKey_pressed, FPtr, LPtr)

    void *Headline;
long long RowN, KolN;
void *InputField;
long long RowUp;
Boolean FirstevKey_pressed;
Boolean (*FPtr)();
Boolean (*LPtr)();
{
  long long i;

  for (i = 0; i < MaxGet; i++)
  {
    if (InFieldPtr[i] == 0)
    {
      InFieldPtr[i] = (struct TakeField *)
          malloc(sizeof(struct TakeField));
      (*InFieldPtr[i]).FieldPtr = (GetStr *)InputField;
      break;
    }
  }
  strcpy((*InFieldPtr[i]).Headline, (char *)Headline);
  (*InFieldPtr[i]).RowN = RowN;
  (*InFieldPtr[i]).KolN = KolN;
  strcpy((*InFieldPtr[i]).InputField, (char *)InputField);
  (*InFieldPtr[i]).RowUp = RowUp;
  (*InFieldPtr[i]).VelPolja = 9;
  (*InFieldPtr[i]).RowLength = 9;
  (*InFieldPtr[i]).ZadevKey_pressed = 0;
  (*InFieldPtr[i]).Azuriran = F;
  (*InFieldPtr[i]).FirstevKey_pressed = FirstevKey_pressed;
  (*InFieldPtr[i]).FunPtr = FPtr;
  (*InFieldPtr[i]).ListPtr = LPtr;
  (*InFieldPtr[i]).Datum = T;
}
/***************************************************************************/
long long
Read()
{
  void ProcessGet();
  long long imaih, Row[MaxGet], Poc[MaxGet];
  long long i, j;
  WINDOW *Wget[MaxGet];
  char out[MasAchRow];

  for (i = 0; InFieldPtr[i] != (struct TakeField *)0; i++)
  {
    if ((*InFieldPtr[i]).RowN == (*InFieldPtr[i]).RowUp)
    {
      Row[i] = 0;
      Poc[i] = strlen((*InFieldPtr[i]).Headline) + 1;
      Wget[i] = newwin(Row[i] + 1, (*InFieldPtr[i]).RowLength + strlen((*InFieldPtr[i]).Headline) + 2,
                       (*InFieldPtr[i]).RowN, (*InFieldPtr[i]).KolN);
      (*InFieldPtr[i]).KolG = (*InFieldPtr[i]).KolN + Poc[i];
    }
    else
    {
      Row[i] = (*InFieldPtr[i]).RowUp - (*InFieldPtr[i]).RowN;
      Poc[i] = 0;
      Wget[i] = newwin(Row[i] + 1, max((*InFieldPtr[i]).RowLength + 1, strlen((*InFieldPtr[i]).Headline)),
                       (*InFieldPtr[i]).RowN, (*InFieldPtr[i]).KolN);
      (*InFieldPtr[i]).KolG = (*InFieldPtr[i]).KolN;
    }
    wattron(Wget[i], COLOR_PAIR(PAIR3));
    mvwaddstr(Wget[i], 0, 0, (*InFieldPtr[i]).Headline);
    mvwaddstr(Wget[i], Row[i], Poc[i],
              vsubstr(out, (*InFieldPtr[i]).InputField, 0, (*InFieldPtr[i]).RowLength));
    wnoutrefresh(Wget[i]);
    overwrite(Wget[i], stdscr);
    wattron(stdscr, COLOR_PAIR(PAIR1));
  }
  imaih = i - 1;
  for (i = 0; i <= imaih;)
  {
    ProcessGet(Wget[i], Row[i], Poc[i], InFieldPtr[i]);
    /* Esc, Ctrl-x, f8 */
    switch ((*InFieldPtr[i]).ZadevKey_pressed)
    {
    case 27:  /* Esc */
    case 24:  /* Ctrl-x */
    case 384: /* f8 */
      for (j = 0; j <= imaih; j++)
      {
        free(InFieldPtr[j]);
        InFieldPtr[j] = (struct TakeField *)0;
        delwin(Wget[j]);
      }
      return (-1);
    case 382: /* f6 */
    case 11:  /* Ctrl-k */
      for (j = 0; j <= imaih; j++)
      {
        free(InFieldPtr[j]);
        InFieldPtr[j] = (struct TakeField *)0;
        delwin(Wget[j]);
      }
      return (-2);
    case 12:  /* Ctrl-l */
    case 385: /* f9 */
      Direct_Exit = F;
      if (((*InFieldPtr[i]).ListPtr == (Boolean(*)())0) ||
          ((*((*InFieldPtr[i]).ListPtr))((*InFieldPtr[i]).InputField,
                                         Row[i], Poc[i], Wget[i]) == F))
      {
        if (Direct_Exit == T)
        {
          for (j = 0; j <= imaih; j++)
          {
            free(InFieldPtr[j]);
            InFieldPtr[j] = (struct TakeField *)0;
            delwin(Wget[j]);
          }
          return (-2);
        }
        continue;
      }
      (*InFieldPtr[i]).Azuriran = T;
      i++;
      break;
    case 13:  /* Enter */
    case 366: /* V */
      Direct_Exit = F;
      if (((*InFieldPtr[i]).FunPtr != (Boolean(*)())0) &&
          ((*((*InFieldPtr[i]).FunPtr))((*InFieldPtr[i]).InputField,
                                        Row[i], Poc[i], Wget[i]) == F))
      {
        if (Direct_Exit == T)
        {
          for (j = 0; j <= imaih; j++)
          {
            free(InFieldPtr[j]);
            InFieldPtr[j] = (struct TakeField *)0;
            delwin(Wget[j]);
          }
          return (-2);
        }
        continue;
      }
      (*InFieldPtr[i]).Azuriran = T;
      i++;
      break;
      /* Help: */
    case 9: /* Ctrl-i */
      wattroff(stdscr, COLOR_PAIR(PAIR1));
      Help((*InFieldPtr[i]).Headline);
      wattron(stdscr, COLOR_PAIR(PAIR1));
      break;
    case 365: /* ^ */
      if (i > 0)
        i--;
      break;
    case 2: /* Ctrl-b */
      if (i > 0)
        i = 0;
      break;
    case 6: /* Ctrl-f*/
      if (i < imaih)
      {
        for (j = i; j < imaih; j++)
        {
          if (((*InFieldPtr[j]).FunPtr != 0) &&
              ((*((*InFieldPtr[j]).FunPtr))((*InFieldPtr[j]).InputField, Row[j], Poc[j], Wget[j]) == F))
          {
            i = j;
            break;
          }
        }
        if (i != j)
          i = imaih;
      }
      else
      {
        if (((*InFieldPtr[i]).FunPtr != 0) &&
            ((*((*InFieldPtr[i]).FunPtr))((*InFieldPtr[i]).InputField, Row[i], Poc[i], Wget[i]) == F))
          continue;
        i++;
      }
      break;
    }
  }
  for (j = 0; j <= imaih; j++)
  {
    if ((*InFieldPtr[j]).Azuriran == T)
      strcpy(*((*InFieldPtr[j]).FieldPtr), (*InFieldPtr[j]).InputField);
    free(InFieldPtr[j]);
    InFieldPtr[j] = (struct TakeField *)0;

    delwin(Wget[j]);
  }
  return (0);
}
/***************************************************************************/
void
    ProcessGet(Wget, Row, Poc, PPtr)

        WINDOW *Wget;
long long Row, Poc;
struct TakeField *PPtr;
{
  long long Taken, RelKol, PocPoz;
  long long i, PositionRead;
  char Kolona[255];
  char out[MasAchRow];
  Boolean FirstevKey_pressed;

  FirstevKey_pressed = PPtr->FirstevKey_pressed; /* Da ne bi promenio PPtr->FirstevKey_pressed */
  PPtr->ZadevKey_pressed = 0;
  PositionRead = 0;
  RelKol = 0;
  PocPoz = 0;
  wattron(Winf, A_STANDOUT);
  mvwaddstr(Winf, 0, 0, " KOL:   1 ");
  if (InsertMod == 1 && PPtr->Datum == F)
    mvwaddstr(Winf, YIns, XIns, " Ins ");
  else
    mvwaddstr(Winf, YIns, XIns, "     ");
  wnoutrefresh(Winf);

  wattron(Wget, A_STANDOUT);
  mvwaddstr(Wget, Row, Poc, vsubstr(out, PPtr->InputField, 0, PPtr->RowLength));
  wmove(Wget, Row, Poc);
  wnoutrefresh(Wget);
  doupdate();
  for (;;)
  {
    Taken = evKey_pressed();
    if (FirstevKey_pressed == T && Taken >= 0) /* Because of irrelevant -1 */
    {
      FirstevKey_pressed = F;
      if (PPtr->Datum == T)
      {
        if (Taken > 31 && Taken < 127)
        {
          for (i = 0; i < PPtr->VelPolja; i++)
          {
            if (i != 2 && i != 5 && i != 8)
              *(PPtr->InputField + i) = ' ';
          }
        }
      }
      else
      {
        if (Taken > 31 && Taken < 127)
        {
          for (i = 0; i < PPtr->VelPolja; i++)
            *(PPtr->InputField + i) = ' ';
        }
      }
      *(PPtr->InputField + PPtr->VelPolja) = '\0';
      mvwaddstr(Wget, Row, Poc, vsubstr(out, PPtr->InputField, 0, PPtr->RowLength));
      wmove(Wget, Row, Poc);
      wrefresh(Wget);
    }
    switch (Taken)
    {
    case 21:  /* Ins */
    case 376: /* Ctrl-U */
      if (PPtr->Datum == F)
      {
        SwIns();
        wmove(Wget, Row, Poc + RelKol);
        wrefresh(Wget);
        continue;
      }
      break;
    case 127: /* BkSpc */
      if (PositionRead > 0)
      {
        PPtr->Azuriran = T;
        PositionRead--;
        if (PPtr->Datum == T)
        {
          if (PositionRead == 2 || PositionRead == 5)
          {
            PositionRead--;
            RelKol--;
            *(PPtr->InputField + PositionRead + 2) = ' ';
          }
          else
          {
            *(PPtr->InputField + PositionRead) =
                *(PPtr->InputField + PositionRead + 1);
            *(PPtr->InputField + PositionRead + 1) = ' ';
          }
        }
        else
        {
          for (i = PositionRead; i < PPtr->VelPolja; i++)
            *(PPtr->InputField + i) = *(PPtr->InputField + i + 1);
          *(PPtr->InputField + PPtr->VelPolja - 1) = ' ';
          *(PPtr->InputField + PPtr->VelPolja) = '\0';
        }
        if (RelKol == 0)
          PocPoz--;
        else
          RelKol--;
      }
      break;
    case 368: /* <---- */
      if (PositionRead > 0)
      {
        PositionRead--;
        if (RelKol == 0)
          PocPoz--;
        else
        {
          if (PPtr->Datum == T && (PositionRead == 2 || PositionRead == 5))
          {
            PositionRead--;
            RelKol--;
          }
          RelKol--;
          sprintf(Kolona, " KOL: %3lld ", PositionRead + 1);
          mvwaddstr(Winf, 0, 0, Kolona);
          wnoutrefresh(Winf);
          wmove(Wget, Row, Poc + RelKol);
          wnoutrefresh(Wget);
          doupdate();
          continue;
        }
      }
      break;
    case 367: /* ----> */
      if (PositionRead < min(strlen(trim(out, PPtr->InputField)), PPtr->VelPolja - 1))
      {
        PositionRead++;
        if (RelKol == PPtr->RowLength - 1)
          PocPoz++;
        else
        {
          if (PPtr->Datum == T)
          {
            if (PositionRead == 2 || PositionRead == 5)
            {
              PositionRead++;
              RelKol++;
            }
            if (PositionRead == 8)
            {
              PositionRead--;
              RelKol--;
            }
          }
          RelKol++;
          sprintf(Kolona, " KOL: %3lld ", PositionRead + 1);
          mvwaddstr(Winf, 0, 0, Kolona);
          wnoutrefresh(Winf);
          wmove(Wget, Row, Poc + RelKol);
          wnoutrefresh(Wget);
          doupdate();
          continue;
        }
      }
      break;
    case 5: /* Ctrl-e */
      PositionRead = min(strlen(trim(out, PPtr->InputField)), PPtr->VelPolja - 1);
      if (strlen(trim(out, PPtr->InputField)) < PPtr->RowLength)
      {
        RelKol = PositionRead;
        PocPoz = 0;
      }
      else
      {
        if (PPtr->Datum == T)
        {
          RelKol = PPtr->RowLength - 2;
          PositionRead--;
        }
        else
          RelKol = PPtr->RowLength - 1;
        PocPoz = strlen(trim(out, PPtr->InputField)) - PPtr->RowLength;
      }
      break;
    case 1: /* Ctrl-a */
      PositionRead = 0;
      RelKol = 0;
      PocPoz = 0;
      break;
    case 12:  /* Ctrl-l */
    case 385: /* f9 */
    case 13:  /* Enter */
    case 365: /* ^ */
    case 366: /* V */
    case 6:   /* Ctrl-f*/
    case 2:   /* Ctrl-b */
    case 9:   /* Ctrl-i */
      PPtr->ZadevKey_pressed = Taken;
      wattroff(Wget, A_STANDOUT);
      mvwaddstr(Wget, Row, Poc, vsubstr(out, PPtr->InputField, PocPoz, PPtr->RowLength));
      wrefresh(Wget);
      wattroff(Winf, A_STANDOUT);
      wattroff(stdscr, COLOR_PAIR(PAIR1));
      overwrite(Wget, stdscr);
      overwrite(Winf, stdscr);
      wattron(stdscr, COLOR_PAIR(PAIR1));
      return;
    case 24:  /* Ctrl-x */
    case 27:  /* Esc */
    case 384: /* f8 */
    case 382: /* f6 */
    case 11:  /* Ctrl-k */
      PPtr->Azuriran = F;
      PPtr->ZadevKey_pressed = Taken;
      wattroff(Wget, A_STANDOUT);
      mvwaddstr(Wget, Row, Poc, vsubstr(out, PPtr->InputField, 0, PPtr->RowLength));
      wrefresh(Wget);
      wattroff(Winf, A_STANDOUT);
      wattroff(stdscr, COLOR_PAIR(PAIR1));
      overwrite(Wget, stdscr);
      overwrite(Winf, stdscr);
      wattron(stdscr, COLOR_PAIR(PAIR1));
      return;
    default:
      if (Taken > 31 && Taken < 127) /* letters and numbers */
      {
        PPtr->Azuriran = T;
        Taken = toupper(Taken);
        if (PPtr->Datum == T)
        {
          if (Taken == '.')
            continue;
          *(PPtr->InputField + PositionRead) = Taken;
          if (PositionRead == 1 || PositionRead == 4)
          {
            PositionRead++;
            RelKol++;
          }
          if (PositionRead == 7)
          {
            PositionRead--;
            RelKol--;
          }
          PositionRead++;
        }
        else
        {
          if (InsertMod == 1)
            for (i = PPtr->VelPolja - 1; i > PositionRead; i--)
              *(PPtr->InputField + i) = *(PPtr->InputField + i - 1);
          *(PPtr->InputField + PositionRead) = Taken;
          PositionRead++;
          if (PositionRead == PPtr->VelPolja)
          {
            PPtr->ZadevKey_pressed = 13;
            wattroff(Wget, A_STANDOUT);
            mvwaddstr(Wget, Row, Poc,
                      vsubstr(out, PPtr->InputField, PocPoz, PPtr->RowLength));
            wrefresh(Wget);
            wattroff(Winf, A_STANDOUT);
            wattroff(stdscr, COLOR_PAIR(PAIR1));
            overwrite(Wget, stdscr);
            overwrite(Winf, stdscr);
            wattron(stdscr, COLOR_PAIR(PAIR1));
            return;
          }
        }
        if (RelKol == PPtr->RowLength - 1)
          PocPoz++;
        else
          RelKol++;
      }
      break;
    }
    sprintf(Kolona, " KOL: %3lld ", PositionRead + 1);
    mvwaddstr(Winf, 0, 0, Kolona);
    wnoutrefresh(Winf);
    mvwaddstr(Wget, Row, Poc, vsubstr(out, PPtr->InputField, PocPoz, PPtr->RowLength));
    wmove(Wget, Row, Poc + RelKol);
    wnoutrefresh(Wget);
    doupdate();
  }
}
/***************************************************************************/
void
    GetSquare(RowN, KolN, InputField, VelPolja, RowLength)

        long long RowN,
    KolN;
void *InputField;
long long VelPolja, RowLength;
{
  long long i;

  for (i = 0; i < MaxGet; i++)
  {
    if (InFieldPtr[i] == 0)
    {
      InFieldPtr[i] = (struct TakeField *)
          malloc(sizeof(struct TakeField));
      (*InFieldPtr[i]).FieldPtr = (GetStr *)InputField;
      break;
    }
  }
  (*InFieldPtr[i]).RowN = RowN;
  (*InFieldPtr[i]).KolN = KolN;
  strcpy((*InFieldPtr[i]).InputField, (char *)InputField);
  (*InFieldPtr[i]).VelPolja = VelPolja;
  (*InFieldPtr[i]).RowLength = RowLength;
  (*InFieldPtr[i]).ZadevKey_pressed = 0;
}
/***************************************************************************/
long long
Look()
{
  void ProcessLook();
  long long imaih, Row[MaxLook], Poc[MaxLook];
  long long i, j;
  WINDOW *Wget[MaxLook];
  char out[MasAchRow];

  for (i = 0; InFieldPtr[i] != (struct TakeField *)0; i++)
  {
    Row[i] = 0;
    Poc[i] = 0;
    Wget[i] = newwin(Row[i] + 1,
                     (*InFieldPtr[i]).RowLength,
                     (*InFieldPtr[i]).RowN, (*InFieldPtr[i]).KolN);
    wattron(Wget[i], COLOR_PAIR(PAIR5));
    (*InFieldPtr[i]).KolG = (*InFieldPtr[i]).KolN + Poc[i];
    mvwaddstr(Wget[i], Row[i], Poc[i], vsubstr(out, (*InFieldPtr[i]).InputField, StartPositionW, (*InFieldPtr[i]).RowLength));
    wnoutrefresh(Wget[i]);
    wattroff(stdscr, COLOR_PAIR(PAIR1));
    overwrite(Wget[i], stdscr);
    wattron(stdscr, COLOR_PAIR(PAIR1));
  }
  imaih = i - 1;
  for (i = 1; i <= imaih;)
  {
    Trajan = F;
    ProcessLook(Wget[i], Row[i], Poc[i], InFieldPtr[i]);
    if (Trajan == T)
    {
      for (j = 0; InFieldPtr[j] != (struct TakeField *)0; j++)
      {
        Row[j] = 0;
        Poc[j] = 0;
        Wget[j] = newwin(Row[j] + 1, (*InFieldPtr[j]).RowLength,
                         (*InFieldPtr[j]).RowN, (*InFieldPtr[j]).KolN);
        wattron(Wget[j], COLOR_PAIR(PAIR5));
        (*InFieldPtr[j]).KolG = (*InFieldPtr[j]).KolN + Poc[j];
        mvwaddstr(Wget[j], Row[j], Poc[j],
                  vsubstr(out, (*InFieldPtr[j]).InputField, StartPositionW,
                          (*InFieldPtr[j]).RowLength));
        wattroff(stdscr, COLOR_PAIR(PAIR1));
        overwrite(Wget[j], stdscr);
        wattron(stdscr, COLOR_PAIR(PAIR1));
      }
      wnoutrefresh(stdscr);
    }
    if (InFieldPtr[i] != NULL)
    {
      switch ((*InFieldPtr[i]).ZadevKey_pressed)
      {
      case 24:  /* Ctrl-x */
      case 27:  /* Esc */
      case 384: /* f8 */
        for (j = 0; j <= imaih; j++)
        {
          free(InFieldPtr[j]);
          InFieldPtr[j] = (struct TakeField *)0;
          delwin(Wget[j]);
        }
        return (-1);
      case 382: /* f6 */
      case 11:  /* Ctrl-k */
        for (j = 0; j <= imaih; j++)
        {
          free(InFieldPtr[j]);
          InFieldPtr[j] = (struct TakeField *)0;
          delwin(Wget[j]);
        }
        return (-2);
      case 71:  /* G */
      case 103: /* g */
      case 2:   /* Ctrl-b */
        for (j = 0; j <= imaih; j++)
        {
          free(InFieldPtr[j]);
          InFieldPtr[j] = (struct TakeField *)0;
          delwin(Wget[j]);
        }
        return (-3);
      case 89:  /* Y */
      case 121: /* y */
      case 6:   /* Ctrl-f*/
        for (j = 0; j <= imaih; j++)
        {
          free(InFieldPtr[j]);
          InFieldPtr[j] = (struct TakeField *)0;
          delwin(Wget[j]);
        }
        return (-4);
      case 13:  /* Enter */
      case 366: /* V */
        if (i < imaih)
          i++;
        break;
      case 9: /* Ctrl-i */
        Help((*InFieldPtr[i]).Headline);
        break;
      case 365: /* ^ */
        if (i > 1)
          i--;
        break;
      case 7: /* Ctrl-g */
        if (i > 1)
          i = 1;
        break;
      case 4: /* Ctrl-d */
        if (i < imaih)
        {
          for (j = i; j < imaih; j++)
            ;
          if (i != j)
            i = imaih;
        }
        break;
      }
    }
  }
  return (0);
}
/***************************************************************************/
void
    ProcessLook(Wget, Row, Poc, PPtr)

        WINDOW *Wget;
long long Row, Poc;
struct TakeField *PPtr;
{
  long long Taken;
  char Kolona[255];
  char out[MasAchRow];

  if (PPtr == NULL)
    return;
  PPtr->ZadevKey_pressed = 0;
  wattron(Winf, A_STANDOUT);
  sprintf(Kolona, " KOL: %3lld ", ReadPositionW + 1);
  mvwaddstr(Winf, 0, 0, Kolona);
  wnoutrefresh(Winf);

  wattrset(Wget, A_STANDOUT | COLOR_PAIR(PAIR6));
  mvwaddstr(Wget, Row, Poc, vsubstr(out, PPtr->InputField, StartPositionW, PPtr->RowLength));
  wmove(Wget, Row, Poc + HowMuchW);
  wnoutrefresh(Wget);
  doupdate();
  for (;;)
  {
    Taken = evKey_pressed();
    switch (Taken)
    {
    case 368: /* <---- */
      if (ReadPositionW > 0)
      {
        ReadPositionW--;
        if (HowMuchW == 0)
          StartPositionW--;
        else
        {
          HowMuchW--;
          sprintf(Kolona, " KOL: %3lld ", ReadPositionW + 1);
          mvwaddstr(Winf, 0, 0, Kolona);
          wnoutrefresh(Winf);
          wmove(Wget, Row, Poc + HowMuchW);
          wnoutrefresh(Wget);
          doupdate();
          continue;
        }
      }
      break;
    case 367: /* ----> */
      if (ReadPositionW < min(strlen(PPtr->InputField), PPtr->VelPolja - 1))
      {
        ReadPositionW++;
        if (HowMuchW == PPtr->RowLength - 1)
          StartPositionW++;
        else
        {
          HowMuchW++;
          sprintf(Kolona, " KOL: %3lld ", ReadPositionW + 1);
          mvwaddstr(Winf, 0, 0, Kolona);
          wnoutrefresh(Winf);
          wmove(Wget, Row, Poc + HowMuchW);
          wnoutrefresh(Wget);
          doupdate();
          continue;
        }
      }
      break;
    case 5: /* Ctrl-e */
      ReadPositionW = min(strlen(PPtr->InputField), PPtr->VelPolja - 1);
      if (strlen(PPtr->InputField) < PPtr->RowLength)
      {
        HowMuchW = ReadPositionW;
        StartPositionW = 0;
      }
      else
      {
        HowMuchW = PPtr->RowLength - 1;
        StartPositionW = strlen(PPtr->InputField) - PPtr->RowLength;
      }
      break;
    case 1: /* Ctrl-a */
      ReadPositionW = 0;
      HowMuchW = 0;
      StartPositionW = 0;
      break;
    case 13:  /* Enter */
    case 365: /* ^ */
    case 366: /* V */
    case 9:   /* Ctrl-i */
    case 4:   /* Ctrl-d */
    case 7:   /* Ctrl-g */
    case 2:   /* Ctrl-b */
    case 6:   /* Ctrl-f*/
      PPtr->ZadevKey_pressed = Taken;
      wattrset(Wget, COLOR_PAIR(PAIR5));
      mvwaddstr(Wget, Row, Poc,
                vsubstr(out, PPtr->InputField, StartPositionW, PPtr->RowLength));
      wrefresh(Wget);
      wattroff(Winf, A_STANDOUT);
      wattroff(stdscr, COLOR_PAIR(PAIR1));
      overwrite(Wget, stdscr);
      overwrite(Winf, stdscr);
      wattron(stdscr, COLOR_PAIR(PAIR1));
      return;
    case 27: /* Esc */
    case 24: /* Ctrl-x */
    case 11: /* Ctrl-k */
      PPtr->ZadevKey_pressed = Taken;
      return;
    default:
      break;
    }
    sprintf(Kolona, " KOL: %3lld ", ReadPositionW + 1);
    mvwaddstr(Winf, 0, 0, Kolona);
    wrefresh(Winf);
    Trajan = T;
    return;
  }
}
/***************************************************************************/
void SwIns()
{
  if (InsertMod == 0)
  {
    InsertMod = 1;
    mvwaddstr(Winf, YIns, XIns, " Ins ");
  }
  else
  {
    InsertMod = 0;
    mvwaddstr(Winf, YIns, XIns, "     ");
  }
  wrefresh(Winf);
}
/***************************************************************************/
void message(char *Text)
{
  long long VARIABLE_IS_NOT_USED Symbol;
  long long Vel;
  char t1[61], t2[61], t3[61];

  wattron(Winf, A_STANDOUT);
  empty_string(t1, 60);
  mvwaddstr(Winf, 0, 12, t1);
  Vel = strlen(Text);
  if (Vel > 60)
  {
    Text[61] = '\0';
    Vel = 60;
  }
  empty_string(t2, (long long)(30 - (Vel / 2)));
  strcpy(t3, t2);
  strcat(t3, Text);
  mvwaddstr(Winf, 0, 12, t3);
  wrefresh(Winf);
  Symbol = evKey_pressed();
  wattroff(Winf, A_STANDOUT);
  mvwaddstr(Winf, 0, 12, empty_string(t1, 60));
  wrefresh(Winf);
}
/***************************************************************************/
Boolean
    answer(Text)

        char *Text;
{
  long long Symbol, Vel;
  char t1[61], t2[61], t3[61];

  wattron(Winf, A_STANDOUT);
  empty_string(t1, 60);
  mvwaddstr(Winf, 0, 12, t1);
  Vel = strlen(Text);
  if (Vel > 60)
  {
    Text[61] = '\0';
    Vel = 60;
  }
  empty_string(t2, (long long)(30 - (Vel / 2)));
  strcpy(t3, t2);
  strcat(t3, Text);
  mvwaddstr(Winf, 0, 12, t3);
  wrefresh(Winf);

  for (;;)
  {
    Symbol = evKey_pressed();
    if (Symbol != 'Y' && Symbol != 'y' && Symbol != 'N' && Symbol != 'n')
    {
      wattron(Winf, A_BLINK);
      mvwaddstr(Winf, 0, 12, empty_string(t1, 60));
      mvwaddstr(Winf, 0, 27, "Answer must be 'Y' ili 'N'");
      wattroff(Winf, A_BLINK);
      wrefresh(Winf);
    }
    else
      break;
  }
  wattroff(Winf, A_STANDOUT);
  mvwaddstr(Winf, 0, 12, empty_string(t1, 60));
  wrefresh(Winf);
  if (Symbol == 'Y' || Symbol == 'y')
    return (T);
  else
    return (F);
}
/***************************************************************************/
WINDOW *
save_on_screen()
{
  WINDOW *Wcuv;

  Wcuv = newwin(0, 0, 0, 0);
  wattroff(stdscr, COLOR_PAIR(PAIR1));
  overwrite(stdscr, Wcuv);
  wattron(stdscr, COLOR_PAIR(PAIR1));
  return (Wcuv);
}
/**************************************************************************/
void
    rest_from_screen(Wcuv)

        WINDOW *Wcuv;
{
  wattroff(stdscr, COLOR_PAIR(PAIR1));
  overwrite(Wcuv, stdscr);
  wattron(stdscr, COLOR_PAIR(PAIR1));
  wnoutrefresh(stdscr);
  delwin(Wcuv);
}
/***************************************************************************/
void
    wDelete_Screen(MyWindow, wUp, wLeft, wDown, wRight)

        WINDOW *MyWindow;
long long wUp, wLeft, wDown, wRight;
{
  char nothing[COLS + 2];
  char out[MasAchRow];
  long long i;

  strcpy(nothing, empty_string(out, wRight - wLeft + 1));
  for (i = wUp; i <= wDown; i++)
    mvwprintw(MyWindow, i, wLeft, "%s", nothing);
  wnoutrefresh(MyWindow);
}
/***************************************************************************/
