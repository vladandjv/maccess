/**************************************************************/
/* Examins key codes                                          */
/* Compile with: cc key.c -lcurses -o key                     */
/**************************************************************/
#define M_TERMINFO
#include <curses.h>
WINDOW *Wkur;
int
main ()
{
  void InitEkran (), TermEkran ();
  int TakeSymbol (), evKey_pressed ();

  InitEkran ();
  clear ();
  refresh ();
  mvprintw (17, 20, "For exit press CTRL-x (24)");
  refresh ();

  for (;;)
    if (evKey_pressed () == 24) /* CTRL-X - izlaz */
      break;
  TermEkran ();
  return 0;
}
/***********************************************************/
int
evKey_pressed ()
{
  int TakeSymbol ();
  int Symbol;

  Symbol = TakeSymbol ();
  mvprintw (19, 20, "Ulaz  = [%3.0d]", Symbol);
  refresh ();
  if (Symbol == 91)
    {
      Symbol = TakeSymbol ();
      Symbol += 300;
      if (Symbol == 384) /* F8 on console; same as CTRL-X */
        Symbol = 24;
    }
  /* Specific for terminals     */
  if (Symbol == 349 || Symbol == 350 || Symbol == 351)
    {
      Symbol = TakeSymbol ();
      if (Symbol > 47 && Symbol < 58)
        Symbol += 400;
      if (Symbol == 455)
        Symbol = 382; /* i.e. F6 on konsole */
      else if (Symbol == 456)
        Symbol = 383; /* i.e. F7 on konsole */
      else if (Symbol == 457)
        Symbol = 24; /* F8 same as CTRL-X */
      else if (Symbol == 449)
        Symbol = 372; /* F17 i.e. Home on konsole */
      else if (Symbol == 450)
        Symbol = 370; /* F18 i.e. End on konsole */
      else if (Symbol == 451)
        Symbol = 373; /* F19 i.e PgUp on koneole */
      else if (Symbol == 452)
        Symbol = 371; /* F20 i.e. pgDn on konsole */
      TakeSymbol ();
    }
  /************ end specific for Unisis terminals *********/
  mvprintw (20, 20, "Exit = [%3.0d]", Symbol);
  refresh ();
  return (Symbol);
}
/******************************************************/
int
TakeSymbol ()
{
  char Symbol;

  Symbol = wgetch (Wkur);
  if (Symbol == 27)
    {
      nodelay (Wkur, TRUE);
      Symbol = wgetch (Wkur);
      nodelay (Wkur, FALSE);
      if (Symbol == -1)
        return (27);
    }
  return ((int) Symbol);
}
/********************************************************/
void
InitEkran ()
{
  initscr ();
  crmode ();
  noecho ();
  raw ();
  nonl ();
  Wkur = newwin (1, 1, 23, 0);
  wclear (Wkur);
}
/*********************************************************/
void
TermEkran ()
{
  clear ();
  refresh ();
  endwin ();
}
/**********************************************************/
