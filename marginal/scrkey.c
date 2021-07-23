/**************************************************************************
 * PROGRAM: SCRKEY.C                                                     *
 * DESCRIPTION: This program returnes codes form open event queue with a  *
 *    relative device (mouse) and a string device (keyboard).             *
 * REMARK: The diference between functions evKey_pressed and Key_pressed  *
 * is that function Key_pressed() works without ev_block(); which is same *
 * as getch() with nodelay(stdscr,TRUE). WHEN NOTHING IS PRESSED AND      *
 * NODELAY IS SET (I.E. THERE ARE NOT EV_BLOCK) BOTH FUNCTIONS (GETCH AND *
 * EV_READ) RETURN -1. THAT IS WHY I USE -1 FOR RETURN WHEN NOTHING       *
 * RELEVANT IS PRESSED EVEN THOUGH THERE IS GETCH WITH NODELAY (OR.       *
 * EV_BLOCK).                                                             *
 * AUTHOR: VLADAN DJORDJEVIC                                              *
 **************************************************************************/
#include "screen.h"

long long
evKey_pressed()
{
  long long Symbol, TakeSymbol();

  Symbol = TakeSymbol(T);
  switch (Symbol)
  {
  case -2: /* Must be -2 because -1 is default in Key_pressed()! */
    return (27);
  case 27:
    Symbol = TakeSymbol(T);
    if (Symbol == 91)
    {
      Symbol = TakeSymbol(T);
      Symbol += 300;
      if (Symbol > 348 && Symbol < 352) /* vt100 */
      {
        Symbol = TakeSymbol(T);
        TakeSymbol(T);
      }
    }
    else if (Symbol == 27)
    {
      Symbol = evKey_pressed();
      return (Symbol);
    }
    break;
  case 91:
    Symbol = TakeSymbol(F);
    if (Symbol == -1)
      return (91);
    Symbol += 300;
    if (Symbol > 348 && Symbol < 352) /* vt100 */
    {
      Symbol = TakeSymbol(T);
      TakeSymbol(T);
    }
    break;
  }
  return (Symbol);
}
long long
Key_pressed()
{
  long long Symbol, TakeSymbol();

  Symbol = TakeSymbol(F);
  switch (Symbol)
  {
  case -2: /* Returns -1 if nothing is press, that is why -2. */
    return (27);
  case 27:
    Symbol = TakeSymbol(T);
    if (Symbol == 91)
    {
      Symbol = TakeSymbol(T);
      Symbol += 300;
      if (Symbol > 348 && Symbol < 352) /* vt100 */
      {
        Symbol = TakeSymbol(T);
        TakeSymbol(T);
      }
    }
    else if (Symbol == 27)
    {
      Symbol = evKey_pressed();
      return (Symbol);
    }
    break;
  case 91:
    Symbol = TakeSymbol(F);
    if (Symbol == -1)
      return (91);
    Symbol += 300;
    if (Symbol > 348 && Symbol < 352) /* vt100 */
    {
      Symbol = TakeSymbol(T);
      TakeSymbol(T);
    }
    break;
  }
  return (Symbol);
}
long long
    TakeSymbol(WAIT)

        Boolean WAIT;
{
  extern WINDOW *Wkur;
  char Symbol;

  if (WAIT == T)
    Symbol = wgetch(Wkur);
  else
  {
    nodelay(Wkur, TRUE);
    Symbol = wgetch(Wkur);
    nodelay(Wkur, FALSE);
  }
  if (Symbol == 27)
  {
    nodelay(Wkur, TRUE);
    Symbol = wgetch(Wkur);
    nodelay(Wkur, FALSE);
    if (Symbol == -1)
      return (-2);
  }
  return ((long long)Symbol);
}
/******************************* END ***************************************/
