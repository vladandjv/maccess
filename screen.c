/**************************************************************************/
/* PROGRAM: SCREEN.C                                                      */
/* DESCRIPTION:                                                           */
/*    Low-level rutines for working with screens                          */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "conf_lib.h"
#include "screen.h"
/************************** constants *************************************/
#define MaxGet 60
#define MaxLook LINES-7       /* Must be less or equal MaxGet-u. */
#define MaxGetLen MAX_STRING_LENGTH
#define MaxHeadlineLen 80

Boolean vtterm = F;
Boolean Direct_Exit = F;
WINDOW *Wsav = NULL;
/****************************** types *************************************/
typedef char GetStr[MaxGetLen];
typedef char HeadlineStr[MaxHeadlineLen];

struct TakeField
{
  GetStr *FieldPtr;
  HeadlineStr Headline;
  int RowN;
  int KolN;
  GetStr InputField;
  int RowUp;
  int KolG;
  int VelPolja;
  int RowLength;
  int ZadevKey_pressed;
  Boolean Azuriran;
  Boolean FirstevKey_pressed;
  Boolean (*FunPtr)();
  Boolean (*ListPtr)();
  Boolean Datum;
};
/*************************  variables *************************************/
int InsertMod;
int XIns;
int YIns;

WINDOW *Winf;
struct TakeField *InFieldPtr[MaxGet];

int StartPositionW, HowMuchW;
long long ReadPositionW;
Boolean Zorica;

#ifdef _HAS_MOUSE
#include <mouse.h> 
#else
WINDOW *Wkur;
#endif /* _HAS_MOUSE */
/************************* FUNCTIONS OF EDITOR ****************************/
void
InitEkran ()
{
  char koji_term[100];
  int i;

#ifdef _HAS_MOUSE
  dmask_t dmask = D_STRING | D_REL | D_BUTTON;
  ev_init ();
  if (ev_open (&dmask) < 0)
    {
      printf ("Ne moze da otvori device-ove koje pronalazi!\n");
      ev_close ();
      exit (1);

      /* ev_suspend();        
         dmask = D_STRING;           
         if(ev_open(&dmask) < 0) 
         {   
         ev_close();
         exit(1);
         } */
    }
#endif /* _HAS_MOUSE */

  for (i = 0; i < MaxGet; i++)
    InFieldPtr[i] = (struct TakeField *) 0;
  strcpy (koji_term, (char *) getenv ("TERM"));
  if (strcmp (left (koji_term, 2), "vt") == 0 ||
      strcmp (left (koji_term, 2), "dt") == 0)
    {
      vtterm = T;
#ifdef  _M_TERMCAP
      setgraf ();
#endif /* _M_TERMCAP */
    }
  else
    vtterm = F;

  initscr ();
  crmode ();
  noecho ();
  raw ();
  nonl ();
  Wsav = newwin (0, 0, 0, 0);
#ifndef _HAS_MOUSE
  Wkur = newwin (1, 1, LINES - 1, COLS - 1);
#endif /* _HAS_MOUSE */
  Winf = newwin (1, COLS - 1, LINES - 1, 0);
  XIns = (COLS - 6);
  YIns = 0;
  InsertMod = 0;
#ifdef _M_TERMINFO

  /* Monochrome monitor:

     HI_WHITE: yellow;
     WHITE:    white,cyan,green;
     GRAY:     gray,brown;
     BLACK:    black,blue,red,magenta. */

  if (start_color () == OK)
    {
      init_pair (PAIR1, COLOR_YELLOW, COLOR_BLACK);
      init_pair (PAIR2, COLOR_WHITE, COLOR_BLUE);
      init_pair (PAIR3, COLOR_GREEN, COLOR_BLACK);
      init_pair (PAIR4, COLOR_CYAN, COLOR_RED);
      init_pair (PAIR5, COLOR_CYAN, COLOR_BLACK);
      init_pair (PAIR6, COLOR_WHITE, COLOR_BLACK);
    }
  wattron (stdscr, COLOR_PAIR (PAIR1));
  wattron (Winf, COLOR_PAIR (PAIR5));
#endif /* _M_TERMINFO */
}
/***************************************************************************/
void
TermEkran ()
{
#ifdef  _M_TERMCAP
  if (vtterm == T)
    settext ();
#endif /* _M_TERMCAP */
  wclear (stdscr);
  wrefresh (stdscr);
  endwin ();
#ifdef _HAS_MOUSE
  ev_close ();
#endif /* _HAS_MOUSE */
}
/***************************************************************************/
int
Achoice (Headline, Matrica, Rows, Sirina, wUp, wLeft, Elements, LastChoice, SlovoI)

char *Headline;
AchMatPtr Matrica;
int Rows, Sirina, wUp, wLeft, Elements, *LastChoice;
Boolean SlovoI;
{
  void wCrtica ();
  int Taken, i, Choice, ChoiceRow, FirsRow;
  WINDOW *Wach;

  Choice = 1;
  FirsRow = 0;
  ChoiceRow = 1;

  Rows = min (Rows, Elements);
  wUp--;
  wLeft--;

  mvaddstr (wUp - 1, wLeft, Headline);
#ifdef _M_TERMINFO
  wnoutrefresh (stdscr);
#else
  wrefresh (stdscr);
#endif /* _M_TERMINFO */
  Wach = newwin (Rows + 2, Sirina + 2, wUp, wLeft);
#ifdef _M_TERMINFO
  wattron (Wach, COLOR_PAIR (PAIR2));
#else
  wattrset (Wach, A_STANDOUT);
#endif /* _M_TERMINFO */
  wKutija (Wach, Rows + 2, Sirina + 2, F);
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
        mvwaddstr (Wach, i + 1, 1, left (Matrica[FirsRow + i], Sirina));
    }
  else
    {
      for (i = 2; i <= Rows; i++)
        mvwaddstr (Wach, i, 1, left (Matrica[i - 1], Sirina));
    }
  if (FirsRow != 0)
    mvwprintw (Wach, 1, Sirina + 1, "%c", '^');
  if (FirsRow != Elements - Rows)
    mvwprintw (Wach, Rows, Sirina + 1, "%c", 'V');
#ifdef _M_TERMINFO
  wattrset (Wach, A_STANDOUT | COLOR_PAIR (PAIR6));
#else
  wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMINFO */
  if (*LastChoice > 0 && *LastChoice <= Elements)
    mvwaddstr (Wach, ChoiceRow, 1, left (Matrica[Choice - 1], Sirina));
  else
    mvwaddstr (Wach, 1, 1, left (Matrica[0], Sirina));
#ifdef _M_TERMINFO
  wattroff (Wach, A_STANDOUT | COLOR_PAIR (PAIR6));
#endif /* _M_TERMINFO */
  wmove (Wach, Rows + 1, Sirina + 1);
#ifdef _M_TERMINFO
  wnoutrefresh (Wach);
  doupdate ();

  wattron (Wach, COLOR_PAIR (PAIR2));
#else
  wrefresh (Wach);
#endif /* _M_TERMINFO */
  for (;;)
    {
      Taken = evKey_pressed ();
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
#ifdef  _M_TERMCAP
          wattrset (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          if (FirsRow != 0)
            mvwprintw (Wach, 1, Sirina + 1, "%c", '^');
          else
            wCrtica (Wach, 1, Sirina + 1);
          if (FirsRow != Elements - Rows)
            mvwprintw (Wach, Rows, Sirina + 1, "%c", 'V');
          else
            wCrtica (Wach, Rows, Sirina + 1);
#ifdef  _M_TERMCAP
          wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
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
#ifdef  _M_TERMCAP
          wattrset (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          if (FirsRow != 0)
            mvwprintw (Wach, 1, Sirina + 1, "%c", '^');
          else
            wCrtica (Wach, 1, Sirina + 1);
          if (FirsRow != Elements - Rows)
            mvwprintw (Wach, Rows, Sirina + 1, "%c", 'V');
          else
            wCrtica (Wach, Rows, Sirina + 1);
#ifdef  _M_TERMCAP
          wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          break;
        case 372: /* Home */
        case 1: /* Ctrl-a */
          Choice = 1;
          FirsRow = 0;
          ChoiceRow = 1;
#ifdef  _M_TERMCAP
          wattrset (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          if (FirsRow != Elements - Rows)
            mvwprintw (Wach, Rows, Sirina + 1, "%c", 'V');
          else
            wCrtica (Wach, Rows, Sirina + 1);
          wCrtica (Wach, 1, Sirina + 1);
#ifdef  _M_TERMCAP
          wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          break;
        case 370: /* End */
        case 5: /* Ctrl-e */
          Choice = Elements;
          FirsRow = Elements - Rows;
          ChoiceRow = Rows;
#ifdef  _M_TERMCAP
          wattrset (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          if (FirsRow != 0)
            mvwprintw (Wach, 1, Sirina + 1, "%c", '^');
          else
            wCrtica (Wach, 1, Sirina + 1);
          wCrtica (Wach, Rows, Sirina + 1);
#ifdef  _M_TERMCAP
          wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          break;
        case 371: /* PgDn */
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
#ifdef  _M_TERMCAP
          wattrset (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          if (FirsRow != 0)
            mvwprintw (Wach, 1, Sirina + 1, "%c", '^');
          else
            wCrtica (Wach, 1, Sirina + 1);
          if (FirsRow != Elements - Rows)
            mvwprintw (Wach, Rows, Sirina + 1, "%c", 'V');
          else
            wCrtica (Wach, Rows, Sirina + 1);
#ifdef  _M_TERMCAP
          wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          break;
        case 373: /* PgUp */
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
#ifdef  _M_TERMCAP
          wattrset (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          if (FirsRow != 0)
            mvwprintw (Wach, 1, Sirina + 1, "%c", '^');
          else
            wCrtica (Wach, 1, Sirina + 1);
          if (FirsRow != Elements - Rows)
            mvwprintw (Wach, Rows, Sirina + 1, "%c", 'V');
          else
            wCrtica (Wach, Rows, Sirina + 1);
#ifdef  _M_TERMCAP
          wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
          break;
        case 13: /* Enter */
#ifndef _HAS_MOUSE
        case 367: /* ----> */
#endif /* _HAS_MOUSE */
          *LastChoice = Choice;
#ifdef _M_TERMINFO
          wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          overwrite (Wach, stdscr);
#ifdef _M_TERMINFO
          wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          delwin (Wach);
          return (Choice);
          /* Interruption: */
        case 27: /* Esc */
        case 24: /* Ctrl-x */
        case 384: /* f8 */
#ifndef _HAS_MOUSE
        case 368: /* <---- */
#endif /* _HAS_MOUSE */
          *LastChoice = Choice;
          delwin (Wach);
          return (-1);
          /* Help: */
        case 377: /* f1 */
        case 383: /* f7 */
        case 9: /* Ctrl-i */
#ifdef _M_TERMINFO
          wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          overwrite (Wach, stdscr);
          Help ("ACHOICE");
#ifdef _M_TERMINFO
          wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          break;
        case 382: /* f6 */
        case 11: /* Ctrl-k */
          *LastChoice = Choice;
          delwin (Wach);
          return (-2);
        default:
          if (Taken > 31 && Taken < 127) /* letters and numbers */
            {
              switch (Taken)
                {
                case 96: /* ` */ /* z */
                  Taken = 64; /* @ */ /* Z */
                  break;
                case 123: /* { */ /* malo   s */
                  Taken = 91; /* [ */ /* S */
                  break;
                case 125: /* } */ /* c */ /* cuskija */
                  Taken = 93; /* ] */ /* C */
                  break;
                case 126: /* ~ */ /* c */ /* cep */
                  Taken = 94; /* ^ */ /*  C */
                  break;
                case 124: /* | */ /* dj */
                  Taken = 92; /* \ */ /* Dj */
                  break;
                }
              Taken = toupper (Taken);
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
#ifdef _M_TERMCAP
          /* Print Screen */
        case 16: /* Ctrl-p */
          PrScr ();
          break;
#endif /* _M_TERMCAP */
        }
#ifdef _M_TERMCAP
      wattron (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
      for (i = 0; i < Rows; i++)
        mvwaddstr (Wach, i + 1, 1, left (Matrica[FirsRow + i], Sirina));
#ifdef _M_TERMINFO
      wattron (Wach, A_STANDOUT | COLOR_PAIR (PAIR6));
#else
      wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMINFO */
      mvwaddstr (Wach, ChoiceRow, 1, left (Matrica[Choice - 1], Sirina));
#ifdef _M_TERMINFO
      wattroff (Wach, A_STANDOUT | COLOR_PAIR (PAIR6));
#endif /* _M_TERMINFO */
      wmove (Wach, Rows + 1, Sirina + 1);
      wrefresh (Wach);
#ifdef _M_TERMINFO
      wattron (Wach, COLOR_PAIR (PAIR2));
#endif /* _M_TERMINFO */
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
#ifdef _M_TERMCAP
  wattron (Wach, A_STANDOUT);
#endif /* _M_TERMCAP */
  for (i = 0; i < Rows; i++)
    mvwaddstr (Wach, i + 1, 1, left (Matrica[FirsRow + i], Sirina));
#ifdef _M_TERMINFO
  wattron (Wach, A_STANDOUT | COLOR_PAIR (PAIR6));
#else
  wattroff (Wach, A_STANDOUT);
#endif /* _M_TERMINFO */
  mvwaddstr (Wach, ChoiceRow, 1, left (Matrica[Choice - 1], Sirina));
#ifdef _M_TERMINFO
  wattroff (Wach, A_STANDOUT | COLOR_PAIR (PAIR6));
#endif /* _M_TERMINFO */
  wmove (Wach, Rows + 1, Sirina + 1);
#ifdef _M_TERMINFO
  wnoutrefresh (Wach);
#else
  wrefresh (Wach);
#endif /* _M_TERMINFO */
  *LastChoice = Choice;
#ifdef _M_TERMINFO
  wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
  overwrite (Wach, stdscr);
#ifdef _M_TERMINFO
  wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
  delwin (Wach);
  return (Choice);
}
/***************************************************************************/
void
wCrtica (MyWindow, wUp, wLeft)

WINDOW *MyWindow;
int wUp, wLeft;
{
#ifdef  _M_TERMCAP
  char ACS_VLINE;

  if (vtterm == T)
    ACS_VLINE = '\x78';
  else
    ACS_VLINE = '\xB3';
#endif /* _M_TERMCAP */

#ifdef _M_TERMINFO  
  wattron (MyWindow, A_ALTCHARSET);
#endif /* _M_TERMINFO */
  mvwprintw (MyWindow, wUp, wLeft, "%c", ACS_VLINE);
#ifdef _M_TERMINFO
  wattroff (MyWindow, A_ALTCHARSET);
  wnoutrefresh (MyWindow);
#endif /* _M_TERMINFO */
}
/***************************************************************************/
void
wKutija (MyWindow, Rows, Sirina, Delete_Screen)

WINDOW *MyWindow;
int Rows, Sirina;
Boolean Delete_Screen;
{
  if (Delete_Screen == T)
    wDelete_Screen (MyWindow, 1, 1, Rows - 2, Sirina - 2);
  wFrame (MyWindow, 0, 0, Rows - 1, Sirina - 1);
}
/***************************************************************************/
void GetField
(Headline, RowN, KolN, InputField, RowUp, VelPolja, RowLength, FirstevKey_pressed, FPtr, LPtr)

void *Headline;
int RowN, KolN;
void *InputField;
int RowUp, VelPolja, RowLength;
Boolean FirstevKey_pressed;
Boolean (*FPtr)();
Boolean (*LPtr)();
{
  int i;

  for (i = 0; i < MaxGet; i++)
    {
      if (InFieldPtr[i] == (struct TakeField *) 0)
        {
          InFieldPtr[i] = (struct TakeField *)
                  malloc (sizeof (struct TakeField));
          (*InFieldPtr[i]).FieldPtr = (GetStr *) InputField;
          break;
        }
    }
  strcpy ((*InFieldPtr[i]).Headline, (char *) Headline);
  (*InFieldPtr[i]).RowN = RowN;
  (*InFieldPtr[i]).KolN = KolN;
  strcpy ((*InFieldPtr[i]).InputField, (char *) InputField);
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
void GetDatum
(Headline, RowN, KolN, InputField, RowUp, FirstevKey_pressed, FPtr, LPtr)

void *Headline;
int RowN, KolN;
void *InputField;
int RowUp;
Boolean FirstevKey_pressed;
Boolean (*FPtr)();
Boolean (*LPtr)();
{
  int i;

  for (i = 0; i < MaxGet; i++)
    {
      if (InFieldPtr[i] == 0)
        {
          InFieldPtr[i] = (struct TakeField *)
                  malloc (sizeof (struct TakeField));
          (*InFieldPtr[i]).FieldPtr = (GetStr *) InputField;
          break;
        }
    }
  strcpy ((*InFieldPtr[i]).Headline, (char *) Headline);
  (*InFieldPtr[i]).RowN = RowN;
  (*InFieldPtr[i]).KolN = KolN;
  strcpy ((*InFieldPtr[i]).InputField, (char *) InputField);
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
int
Read ()
{
  void ProcessGet ();
  int imaih, Row[MaxGet], Poc[MaxGet];
  int i, j;
  WINDOW * Wget[MaxGet];

  for (i = 0; InFieldPtr[i] != (struct TakeField *) 0; i++)
    {
      if ((*InFieldPtr[i]).RowN == (*InFieldPtr[i]).RowUp)
        {
          Row[i] = 0;
          Poc[i] = strlen ((*InFieldPtr[i]).Headline) + 1;
          Wget[i] = newwin (Row[i] + 1, (*InFieldPtr[i]).RowLength +
                            strlen ((*InFieldPtr[i]).Headline) + 2,
                            (*InFieldPtr[i]).RowN, (*InFieldPtr[i]).KolN);
          (*InFieldPtr[i]).KolG = (*InFieldPtr[i]).KolN + Poc[i];
        }
      else
        {
          Row[i] = (*InFieldPtr[i]).RowUp - (*InFieldPtr[i]).RowN;
          Poc[i] = 0;
          Wget[i] = newwin (Row[i] + 1, max ((*InFieldPtr[i]).RowLength + 1,
                                             strlen ((*InFieldPtr[i]).Headline)),
                            (*InFieldPtr[i]).RowN, (*InFieldPtr[i]).KolN);
          (*InFieldPtr[i]).KolG = (*InFieldPtr[i]).KolN;
        }
#ifdef _M_TERMINFO
      wattron (Wget[i], COLOR_PAIR (PAIR3));
#endif /* _M_TERMINFO */
      mvwaddstr (Wget[i], 0, 0, (*InFieldPtr[i]).Headline);
      mvwaddstr (Wget[i], Row[i], Poc[i],
                 vsubstr ((*InFieldPtr[i]).InputField, 0, (*InFieldPtr[i]).RowLength));
#ifdef _M_TERMINFO             
      wnoutrefresh (Wget[i]);
#else
      wrefresh (stdscr);
      wrefresh (Wget[i]);
#endif /* _M_TERMINFO */
      overwrite (Wget[i], stdscr);
#ifdef _M_TERMINFO
      wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
    }
  imaih = i - 1;
  for (i = 0; i <= imaih;)
    {
      ProcessGet (Wget[i], Row[i], Poc[i], InFieldPtr[i]);
      /* Esc, Ctrl-x, f8 */
      switch ((*InFieldPtr[i]).ZadevKey_pressed)
        {
        case 27: /* Esc */
        case 24: /* Ctrl-x */
        case 384: /* f8 */
          for (j = 0; j <= imaih; j++)
            {
              InFieldPtr[j] = (struct TakeField *) 0;
              free (InFieldPtr[j]);
              delwin (Wget[j]);
            }
          return (-1);
        case 382: /* f6 */
        case 11: /* Ctrl-k */
          for (j = 0; j <= imaih; j++)
            {
              InFieldPtr[j] = (struct TakeField *) 0;
              free (InFieldPtr[j]);
              delwin (Wget[j]);
            }
          return (-2);
        case 12: /* Ctrl-l */
        case 385: /* f9 */
          Direct_Exit = F;
          if (((*InFieldPtr[i]).ListPtr == (Boolean (*)()) 0) ||
              ((*((*InFieldPtr[i]).ListPtr))((*InFieldPtr[i]).InputField,
                                             Row[i], Poc[i], Wget[i]) == F))
            {
              if (Direct_Exit == T)
                {
                  for (j = 0; j <= imaih; j++)
                    {
                      InFieldPtr[j] = (struct TakeField *) 0;
                      free (InFieldPtr[j]);
                      delwin (Wget[j]);
                    }
                  return (-2);
                }
              continue;
            }
          (*InFieldPtr[i]).Azuriran = T;
          i++;
          break;
        case 13: /* Enter */
        case 366: /* V */
          Direct_Exit = F;
          if (((*InFieldPtr[i]).FunPtr != (Boolean (*)()) 0) &&
              ((*((*InFieldPtr[i]).FunPtr))((*InFieldPtr[i]).InputField,
                                            Row[i], Poc[i], Wget[i]) == F))
            {
              if (Direct_Exit == T)
                {
                  for (j = 0; j <= imaih; j++)
                    {
                      InFieldPtr[j] = (struct TakeField *) 0;
                      free (InFieldPtr[j]);
                      delwin (Wget[j]);
                    }
                  return (-2);
                }
              continue;
            }
          (*InFieldPtr[i]).Azuriran = T;
          i++;
          break;
          /* Help: */
        case 377: /* f1 */
        case 383: /* f7 */
        case 9: /* Ctrl-i */
#ifdef _M_TERMINFO
          wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          Help ((*InFieldPtr[i]).Headline);
#ifdef _M_TERMINFO
          wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          break;
        case 365: /* ^ */
          if (i > 0)
            i--;
          break;
        case 373: /* PgUp */
        case 2: /* Ctrl-b */
          if (i > 0)
            i = 0;
          break;
        case 371: /* PgDn */
        case 6: /* Ctrl-f*/
          if (i < imaih)
            {
              for (j = i; j < imaih; j++)
                {
                  if (((*InFieldPtr[j]).FunPtr != 0) &&
                      ((*((*InFieldPtr[j]).FunPtr))
                       ((*InFieldPtr[j]).InputField, Row[j], Poc[j], Wget[j]) == F))
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
                  ((*((*InFieldPtr[i]).FunPtr))
                   ((*InFieldPtr[i]).InputField, Row[i], Poc[i], Wget[i]) == F))
                continue;
              i++;
            }
          break;
        }
    }
  for (j = 0; j <= imaih; j++)
    {
      if ((*InFieldPtr[j]).Azuriran == T)
        strcpy (*((*InFieldPtr[j]).FieldPtr), (*InFieldPtr[j]).InputField);
      InFieldPtr[j] = (struct TakeField *) 0;
      free (InFieldPtr[j]);

      delwin (Wget[j]);
    }
  return (0);
}
/***************************************************************************/
void
ProcessGet (Wget, Row, Poc, PPtr)

WINDOW *Wget;
int Row, Poc;
struct TakeField *PPtr;
{
  int Taken, RelKol, PocPoz;
  long long i, PositionRead;
  char Kolona[255];
  Boolean FirstevKey_pressed;

  FirstevKey_pressed = PPtr->FirstevKey_pressed; /* Da ne bi promenio PPtr->FirstevKey_pressed */
  PPtr->ZadevKey_pressed = 0;
  PositionRead = 0;
  RelKol = 0;
  PocPoz = 0;
  wattron (Winf, A_STANDOUT);
  mvwaddstr (Winf, 0, 0, " KOL:   1 ");
  if (InsertMod == 1 && PPtr->Datum == F)
    mvwaddstr (Winf, YIns, XIns, " Ins ");
  else
    mvwaddstr (Winf, YIns, XIns, "     ");
#ifdef _M_TERMINFO
  wnoutrefresh (Winf);
#else
  wrefresh (Winf);
#endif /* _M_TERMINFO */

  wattron (Wget, A_STANDOUT);
  mvwaddstr (Wget, Row, Poc, vsubstr (PPtr->InputField, 0, PPtr->RowLength));
  wmove (Wget, Row, Poc);
#ifdef _M_TERMINFO
  wnoutrefresh (Wget);
  doupdate ();
#else
  wrefresh (Wget);
#endif /* _M_TERMINFO */
  for (;;)
    {
      Taken = evKey_pressed ();
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
          mvwaddstr (Wget, Row, Poc, vsubstr (PPtr->InputField, 0, PPtr->RowLength));
          wmove (Wget, Row, Poc);
          wrefresh (Wget);
        }
      switch (Taken)
        {
        case 21: /* Ins */
        case 376: /* Ctrl-U */
          if (PPtr->Datum == F)
            {
              SwIns ();
              wmove (Wget, Row, Poc + RelKol);
              wrefresh (Wget);
              continue;
            }
          break;
        case 8: /* BkSpc */
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
        case 127: /* Del */
          PPtr->Azuriran = T;
          if (PPtr->Datum == T)
            {
              switch (PositionRead)
                {
                case 1:
                case 4:
                case 7:
                  *(PPtr->InputField + PositionRead) = ' ';
                  break;
                default:
                  *(PPtr->InputField + PositionRead) = *(PPtr->InputField + PositionRead + 1);
                  *(PPtr->InputField + PositionRead + 1) = ' ';
                  break;
                }
            }
          else
            {
              for (i = PositionRead; i < PPtr->VelPolja; i++)
                *(PPtr->InputField + i) = *(PPtr->InputField + i + 1);
              *(PPtr->InputField + PPtr->VelPolja - 1) = ' ';
              *(PPtr->InputField + PPtr->VelPolja) = '\0';
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
                  sprintf (Kolona, " KOL: %3lld ", PositionRead + 1);
                  mvwaddstr (Winf, 0, 0, Kolona);
#ifdef _M_TERMINFO
                  wnoutrefresh (Winf);
#else
                  wrefresh (Winf);
#endif /* _M_TERMINFO */
                  wmove (Wget, Row, Poc + RelKol);
#ifdef _M_TERMINFO
                  wnoutrefresh (Wget);
                  doupdate ();
#else
                  wrefresh (Wget);
#endif /* _M_TERMINFO */
                  continue;
                }
            }
          break;
        case 367: /* ----> */
          if (PositionRead < min (strlen (trim (PPtr->InputField)), PPtr->VelPolja - 1))
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
                  sprintf (Kolona, " KOL: %3lld ", PositionRead + 1);
                  mvwaddstr (Winf, 0, 0, Kolona);
#ifdef _M_TERMINFO
                  wnoutrefresh (Winf);
#else
                  wrefresh (Winf);
#endif /* _M_TERMINFO */
                  wmove (Wget, Row, Poc + RelKol);
#ifdef _M_TERMINFO
                  wnoutrefresh (Wget);
                  doupdate ();
#else
                  wrefresh (Wget);
#endif /* _M_TERMINFO */
                  continue;
                }
            }
          break;
        case 370: /* End */
        case 5: /* Ctrl-e */
          PositionRead = min (strlen (trim (PPtr->InputField)), PPtr->VelPolja - 1);
          if (strlen (trim (PPtr->InputField)) < PPtr->RowLength)
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
              PocPoz = strlen (trim (PPtr->InputField)) - PPtr->RowLength;
            }
          break;
        case 372: /* Home */
        case 1: /* Ctrl-a */
          PositionRead = 0;
          RelKol = 0;
          PocPoz = 0;
          break;
        case 12: /* Ctrl-l */
        case 385: /* f9 */
        case 13: /* Enter */
        case 365: /* ^ */
        case 366: /* V */
        case 371: /* PgDn */
        case 6: /* Ctrl-f*/
        case 373: /* PgUp */
        case 2: /* Ctrl-b */
        case 377: /* f1 */
        case 383: /* f7 */
        case 9: /* Ctrl-i */
          PPtr->ZadevKey_pressed = Taken;
          wattroff (Wget, A_STANDOUT);
          mvwaddstr (Wget, Row, Poc, vsubstr (PPtr->InputField, PocPoz, PPtr->RowLength));
          wrefresh (Wget);
          wattroff (Winf, A_STANDOUT);
#ifdef _M_TERMINFO
          wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          overwrite (Wget, stdscr);
          overwrite (Winf, stdscr);
#ifdef _M_TERMINFO
          wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          return;
        case 24: /* Ctrl-x */
        case 27: /* Esc */
        case 384: /* f8 */
        case 382: /* f6 */
        case 11: /* Ctrl-k */
          PPtr->Azuriran = F;
          PPtr->ZadevKey_pressed = Taken;
          wattroff (Wget, A_STANDOUT);
          mvwaddstr (Wget, Row, Poc, vsubstr (PPtr->InputField, 0, PPtr->RowLength));
          wrefresh (Wget);
          wattroff (Winf, A_STANDOUT);
#ifdef _M_TERMINFO
          wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          overwrite (Wget, stdscr);
          overwrite (Winf, stdscr);
#ifdef _M_TERMINFO
          wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          return;
#ifdef _M_TERMCAP
          /* Print Screen */
        case 16: /* Ctrl-p */
          overwrite (Wget, stdscr);
          overwrite (Winf, stdscr);
          wrefresh (stdscr);
          PrScr ();
          break;
#endif /* _M_TERMCAP */
        default:
          if (Taken > 31 && Taken < 127) /* slova i brojevi */
            {
              PPtr->Azuriran = T;
              switch (Taken)
                {
                case 96: /* ` */ /* z */
                  Taken = 64; /* @ */ /* Z */
                  break;
                case 123: /* { */ /*  s */
                  Taken = 91; /* [ */ /* S */
                  break;
                case 125: /* } */ /* c */ /* cuskija */
                  Taken = 93; /* ] */ /*  C */
                  break;
                case 126: /* ~ */ /*  c */ /* cep */
                  Taken = 94; /* ^ */ /* C */
                  break;
                case 124: /* | */ /* dj */
                  Taken = 92; /* \ */ /* Dj */
                  break;
                }
              Taken = toupper (Taken);
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
                      wattroff (Wget, A_STANDOUT);
                      mvwaddstr (Wget, Row, Poc,
                                 vsubstr (PPtr->InputField, PocPoz, PPtr->RowLength));
                      wrefresh (Wget);
                      wattroff (Winf, A_STANDOUT);
#ifdef _M_TERMINFO
                      wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
                      overwrite (Wget, stdscr);
                      overwrite (Winf, stdscr);
#ifdef _M_TERMINFO
                      wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
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
      sprintf (Kolona, " KOL: %3lld ", PositionRead + 1);
      mvwaddstr (Winf, 0, 0, Kolona);
#ifdef _M_TERMINFO
      wnoutrefresh (Winf);
#else
      wrefresh (Winf);
#endif /* _M_TERMINFO */
      mvwaddstr (Wget, Row, Poc, vsubstr (PPtr->InputField, PocPoz, PPtr->RowLength));
      wmove (Wget, Row, Poc + RelKol);
#ifdef _M_TERMINFO
      wnoutrefresh (Wget);
      doupdate ();
#else
      wrefresh (Wget);
#endif /* _M_TERMINFO */
    }
}
/***************************************************************************/
void
GetSquare (RowN, KolN, InputField, VelPolja, RowLength)

int RowN, KolN;
void *InputField;
int VelPolja, RowLength;
{
  int i;

  for (i = 0; i < MaxGet; i++)
    {
      if (InFieldPtr[i] == 0)
        {
          InFieldPtr[i] = (struct TakeField *)
                  malloc (sizeof (struct TakeField));
          (*InFieldPtr[i]).FieldPtr = (GetStr *) InputField;
          break;
        }
    }
  (*InFieldPtr[i]).RowN = RowN;
  (*InFieldPtr[i]).KolN = KolN;
  strcpy ((*InFieldPtr[i]).InputField, (char *) InputField);
  (*InFieldPtr[i]).VelPolja = VelPolja;
  (*InFieldPtr[i]).RowLength = RowLength;
  (*InFieldPtr[i]).ZadevKey_pressed = 0;
}
/***************************************************************************/
int
Look ()
{
  void ProcessLook ();
  int imaih, Row[MaxLook], Poc[MaxLook];
  int i, j;
  WINDOW * Wget[MaxLook];

  for (i = 0; InFieldPtr[i] != (struct TakeField *) 0; i++)
    {
      Row[i] = 0;
      Poc[i] = 0;
      Wget[i] = newwin (Row[i] + 1,
                        (*InFieldPtr[i]).RowLength,
                        (*InFieldPtr[i]).RowN, (*InFieldPtr[i]).KolN);
#ifdef _M_TERMINFO
      wattron (Wget[i], COLOR_PAIR (PAIR5));
#endif /* _M_TERMINFO */
      (*InFieldPtr[i]).KolG = (*InFieldPtr[i]).KolN + Poc[i];
      mvwaddstr (Wget[i], Row[i], Poc[i], vsubstr ((*InFieldPtr[i]).InputField,
                                                   StartPositionW, (*InFieldPtr[i]).RowLength));
#ifdef _M_TERMINFO
      wnoutrefresh (Wget[i]);
      wattroff (stdscr, COLOR_PAIR (PAIR1));
#else
      wrefresh (stdscr);
      wrefresh (Wget[i]);
#endif /* _M_TERMINFO */
      overwrite (Wget[i], stdscr);
#ifdef _M_TERMINFO
      wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
    }
  imaih = i - 1;
  for (i = 1; i <= imaih;)
    {
      Zorica = F;
      ProcessLook (Wget[i], Row[i], Poc[i], InFieldPtr[i]);
      if (Zorica == T)
        {
          for (j = 0; InFieldPtr[j] != (struct TakeField *) 0; j++)
            {
              Row[j] = 0;
              Poc[j] = 0;
              Wget[j] = newwin (Row[j] + 1, (*InFieldPtr[j]).RowLength,
                                (*InFieldPtr[j]).RowN, (*InFieldPtr[j]).KolN);
#ifdef _M_TERMINFO
              wattron (Wget[j], COLOR_PAIR (PAIR5));
#endif /* _M_TERMINFO */
              (*InFieldPtr[j]).KolG = (*InFieldPtr[j]).KolN + Poc[j];
              mvwaddstr (Wget[j], Row[j], Poc[j],
                         vsubstr ((*InFieldPtr[j]).InputField, StartPositionW,
                                  (*InFieldPtr[j]).RowLength));
#ifdef _M_TERMINFO
              wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
              overwrite (Wget[j], stdscr);
#ifdef _M_TERMINFO
              wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
            }
#ifdef _M_TERMINFO
          wnoutrefresh (stdscr);
#else
          wrefresh (stdscr);
#endif /* _M_TERMINFO */
        }
      if (InFieldPtr[i] != NULL)
        {
          switch ((*InFieldPtr[i]).ZadevKey_pressed)
            {
            case 24: /* Ctrl-x */
            case 27: /* Esc */
            case 384: /* f8 */
              for (j = 0; j <= imaih; j++)
                {
                  InFieldPtr[j] = (struct TakeField *) 0;
                  free (InFieldPtr[j]);
                  delwin (Wget[j]);
                }
              return (-1);
            case 382: /* f6 */
            case 11: /* Ctrl-k */
              for (j = 0; j <= imaih; j++)
                {
                  InFieldPtr[j] = (struct TakeField *) 0;
                  free (InFieldPtr[j]);
                  delwin (Wget[j]);
                }
              return (-2);
            case 71: /* G */
            case 103: /* g */
            case 373: /* PgUp */
            case 2: /* Ctrl-b */
              for (j = 0; j <= imaih; j++)
                {
                  InFieldPtr[j] = (struct TakeField *) 0;
                  free (InFieldPtr[j]);
                  delwin (Wget[j]);
                }
              return (-3);
            case 89: /* Y */
            case 121: /* y */
            case 371: /* PgDn */
            case 6: /* Ctrl-f*/
              for (j = 0; j <= imaih; j++)
                {
                  InFieldPtr[j] = (struct TakeField *) 0;
                  free (InFieldPtr[j]);
                  delwin (Wget[j]);
                }
              return (-4);
            case 13: /* Enter */
            case 366: /* V */
              if (i < imaih)
                i++;
              break;
            case 377: /* f1 */
            case 383: /* f7 */
            case 9: /* Ctrl-i */
              Help ((*InFieldPtr[i]).Headline);
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
                  for (j = i; j < imaih; j++);
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
ProcessLook (Wget, Row, Poc, PPtr)

WINDOW *Wget;
int Row, Poc;
struct TakeField *PPtr;
{
  int Taken;
  char Kolona[255];

  if (PPtr == NULL)
    return;
  PPtr->ZadevKey_pressed = 0;
  wattron (Winf, A_STANDOUT);
  sprintf (Kolona, " KOL: %3lld ", ReadPositionW + 1);
  mvwaddstr (Winf, 0, 0, Kolona);
#ifdef _M_TERMINFO
  wnoutrefresh (Winf);

  wattrset (Wget, A_STANDOUT | COLOR_PAIR (PAIR6));
#else
  wrefresh (Winf);

  wattrset (Wget, A_STANDOUT);
#endif /* _M_TERMINFO */
  mvwaddstr (Wget, Row, Poc, vsubstr (PPtr->InputField, StartPositionW, PPtr->RowLength));
  wmove (Wget, Row, Poc + HowMuchW);
#ifdef _M_TERMINFO
  wnoutrefresh (Wget);
  doupdate ();
#else
  wrefresh (Wget);
#endif /* _M_TERMINFO */
  for (;;)
    {
      Taken = evKey_pressed ();
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
                  sprintf (Kolona, " KOL: %3lld ", ReadPositionW + 1);
                  mvwaddstr (Winf, 0, 0, Kolona);
#ifdef _M_TERMINFO
                  wnoutrefresh (Winf);
#else
                  wrefresh (Winf);
#endif /* _M_TERMINFO */
                  wmove (Wget, Row, Poc + HowMuchW);
#ifdef _M_TERMINFO
                  wnoutrefresh (Wget);
                  doupdate ();
#else
                  wrefresh (Wget);
#endif /* _M_TERMINFO */
                  continue;
                }
            }
          break;
        case 367: /* ----> */
          if (ReadPositionW < min (strlen (PPtr->InputField), PPtr->VelPolja - 1))
            {
              ReadPositionW++;
              if (HowMuchW == PPtr->RowLength - 1)
                StartPositionW++;
              else
                {
                  HowMuchW++;
                  sprintf (Kolona, " KOL: %3lld ", ReadPositionW + 1);
                  mvwaddstr (Winf, 0, 0, Kolona);
#ifdef _M_TERMINFO
                  wnoutrefresh (Winf);
#else
                  wrefresh (Winf);
#endif /* _M_TERMINFO */
                  wmove (Wget, Row, Poc + HowMuchW);
#ifdef _M_TERMINFO
                  wnoutrefresh (Wget);
                  doupdate ();
#else
                  wrefresh (Wget);
#endif /* _M_TERMINFO */
                  continue;
                }
            }
          break;
        case 370: /* End */
        case 5: /* Ctrl-e */
          ReadPositionW = min (strlen (PPtr->InputField), PPtr->VelPolja - 1);
          if (strlen (PPtr->InputField) < PPtr->RowLength)
            {
              HowMuchW = ReadPositionW;
              StartPositionW = 0;
            }
          else
            {
              HowMuchW = PPtr->RowLength - 1;
              StartPositionW = strlen (PPtr->InputField) - PPtr->RowLength;
            }
          break;
        case 372: /* Home */
        case 1: /* Ctrl-a */
          ReadPositionW = 0;
          HowMuchW = 0;
          StartPositionW = 0;
          break;
        case 13: /* Enter */
        case 365: /* ^ */
        case 366: /* V */
        case 383: /* f7 */
        case 377: /* f1 */
        case 9: /* Ctrl-i */
        case 4: /* Ctrl-d */
        case 7: /* Ctrl-g */
        case 71: /* G */
        case 103: /* g */
        case 68: /* D */
        case 100: /* d */
        case 373: /* PgUp */
        case 2: /* Ctrl-b */
        case 371: /* PgDn */
        case 6: /* Ctrl-f*/
          PPtr->ZadevKey_pressed = Taken;
#ifdef _M_TERMINFO
          wattrset (Wget, COLOR_PAIR (PAIR5));
#else
          wattroff (Wget, A_STANDOUT);
#endif /* _M_TERMINFO */
          mvwaddstr (Wget, Row, Poc,
                     vsubstr (PPtr->InputField, StartPositionW, PPtr->RowLength));
          wrefresh (Wget);
          wattroff (Winf, A_STANDOUT);
#ifdef _M_TERMINFO
          wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          overwrite (Wget, stdscr);
          overwrite (Winf, stdscr);
#ifdef _M_TERMINFO
          wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
          return;
        case 27: /* Esc */
        case 24: /* Ctrl-x */
        case 384: /* f8 */
        case 382: /* f6 */
        case 11: /* Ctrl-k */
          PPtr->ZadevKey_pressed = Taken;
          return;
#ifdef _M_TERMCAP
          /* Print Screen */
        case 16: /* Ctrl-p */
          PrScr ();
          continue;
#endif /* _M_TERMCAP */
        default: break;
        }
      sprintf (Kolona, " KOL: %3lld ", ReadPositionW + 1);
      mvwaddstr (Winf, 0, 0, Kolona);
      wrefresh (Winf);
      Zorica = T;
      return;
    }
}
/***************************************************************************/
void
SwIns ()
{
  if (InsertMod == 0)
    {
      InsertMod = 1;
      mvwaddstr (Winf, YIns, XIns, " Ins ");
    }
  else
    {
      InsertMod = 0;
      mvwaddstr (Winf, YIns, XIns, "     ");
    }
  wrefresh (Winf);
}
/***************************************************************************/
void
message (char *Text)
{
  int VARIABLE_IS_NOT_USED Symbol;
  int Vel;
  char *t1, *t2, *t3;

  wattron (Winf, A_STANDOUT);
  t1 = empty_string (60);
  mvwaddstr (Winf, 0, 12, t1);
  Vel = strlen (Text);
  if (Vel > 60)
    {
      Text[61] = '\0';
      Vel = 60;
    }
  t2 = empty_string ((int) (30 - (Vel / 2)));
  t3 = (char *) malloc (61);
  strcpy (t3, t2);
  strcat (t3, Text);
#ifdef _M_TERMCAP
  settext ();
#endif
  mvwaddstr (Winf, 0, 12, t3);
  wrefresh (Winf);
#ifdef _M_TERMCAP
  setgraf ();
#endif /* _M_TERMCAP */
  free (t1);
  free (t2);
  free (t3);

#ifdef _HAS_MOUSE
  do
    {
      Symbol = evKey_pressed ();
#ifdef _M_TERMCAP
      if (Symbol == 16) /* Ctrl-p  Print Screen */
        {
          overwrite (Winf, stdscr);
          wrefresh (stdscr);
          PrScr ();
          Symbol = 0;
          continue;
        }
#endif /* _M_TERMCAP */
    }
  while (Symbol == 0 || (Symbol > 364 && Symbol < 369));
#else
#ifdef _M_TERMCAP
  do
    {
      Symbol = evKey_pressed ();
      if (Symbol == 16) /* Ctrl-p  Print Screen */
        {
          overwrite (Winf, stdscr);
          wrefresh (stdscr);
          PrScr ();
          continue;
        }
    }
  while (Symbol == 16);
#else
  Symbol = evKey_pressed ();
#endif /* _M_TERMCAP */
#endif /* _HAS_MOUSE */
  wattroff (Winf, A_STANDOUT);
  mvwaddstr (Winf, 0, 12, empty_string (60));
  wrefresh (Winf);
}
/***************************************************************************/
Boolean
answer (Text)

char *Text;
{
  int Symbol, Vel;
  char *t1, *t2, *t3;

  wattron (Winf, A_STANDOUT);
  t1 = empty_string (60);
  mvwaddstr (Winf, 0, 12, t1);
  Vel = strlen (Text);
  if (Vel > 60)
    {
      Text[61] = '\0';
      Vel = 60;
    }
  t2 = empty_string ((int) (30 - (Vel / 2)));
  t3 = (char *) malloc (61);
  strcpy (t3, t2);
  strcat (t3, Text);
#ifdef _M_TERMCAP
  settext ();
#endif
  mvwaddstr (Winf, 0, 12, t3);
  wrefresh (Winf);
#ifdef _M_TERMCAP
  setgraf ();
#endif /* _M_TERMCAP */
  free (t1);
  free (t2);
  free (t3);

  for (;;)
    {
      Symbol = evKey_pressed ();
      if (Symbol != 'Y' && Symbol != 'y' && Symbol != 'N' && Symbol != 'n')
        {
#ifdef _M_TERMCAP
          if (Symbol == 16) /* Ctrl-p  Print Screen */
            {
              overwrite (Winf, stdscr);
              wrefresh (stdscr);
              PrScr ();
              continue;
            }
          settext ();
#endif /* _M_TERMCAP */
          wattron (Winf, A_BLINK);
          mvwaddstr (Winf, 0, 12, empty_string (60));
          mvwaddstr (Winf, 0, 27, "Answer must be 'Y' ili 'N'");
          wattroff (Winf, A_BLINK);
          wrefresh (Winf);
#ifdef _M_TERMCAP
          setgraf ();
#endif /* _M_TERMCAP */
        }
      else
        break;
    }
  wattroff (Winf, A_STANDOUT);
  mvwaddstr (Winf, 0, 12, empty_string (60));
  wrefresh (Winf);
  if (Symbol == 'Y' || Symbol == 'y')
    return (T);
  else
    return (F);
}
/***************************************************************************/
WINDOW *
save_on_screen ()
{
  WINDOW *Wcuv;

  Wcuv = newwin (0, 0, 0, 0);
#ifdef _M_TERMINFO
  wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
  overwrite (stdscr, Wcuv);
#ifdef _M_TERMINFO
  wattron (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
  return (Wcuv);
}
/**************************************************************************/
void
rest_from_screen (Wcuv)

WINDOW *Wcuv;
{
#ifdef _M_TERMINFO
  wattroff (stdscr, COLOR_PAIR (PAIR1));
#endif /* _M_TERMINFO */
  overwrite (Wcuv, stdscr);
#ifdef _M_TERMINFO
  wattron (stdscr, COLOR_PAIR (PAIR1));
  wnoutrefresh (stdscr);
#else
  wrefresh (stdscr);
#endif /* _M_TERMINFO */
  delwin (Wcuv);
}
/***************************************************************************/
void
wDelete_Screen (MyWindow, wUp, wLeft, wDown, wRight)

WINDOW *MyWindow;
int wUp, wLeft, wDown, wRight;
{
  char nothing[COLS + 2];
  int i;

  strcpy (nothing, empty_string (wRight - wLeft + 1));
  for (i = wUp; i <= wDown; i++)
    mvwprintw (MyWindow, i, wLeft, "%s", nothing);
#ifdef _M_TERMINFO
  wnoutrefresh (MyWindow);
#endif /* _M_TERMINFO */
}
/*****************************  KRAJ  **************************************/
#ifdef _M_TERMCAP
/***************************************************************************/
void
setgraf ()
{
  if (vtterm == T)
    fprintf (stdout, "\x0E");
}
/***************************************************************************/
void
settext ()
{
  if (vtterm == T)
    fprintf (stdout, "\x0F");
}
/***************************************************************************/
void
PrScr ()
{
  FILE *fp;
  char namef[10];
  char stamp[50];

  strcpy (namef, "XXXXXX");
  mktemp (namef);
  fp = fopen (namef, "w+");
  dmpwin (stdscr, fp, 0);
  fclose (fp);
  strcpy (stamp, "lp -c ");
  strcat (stamp, namef);
  strcat (stamp, " > /dev/null");
  system (stamp);
  strcpy (stamp, "rm ");
  strcat (stamp, namef);
  system (stamp);
}
/*****************************  KRAJ  **************************************/
#endif /* _M_TERMCAP */
