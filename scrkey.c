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
#include "scrdefs.h"
#ifdef _HAS_MOUSE
#include <mouse.h>    /* includes /sys/event.h */
int
evKey_pressed ()
{
  if (!(ev_block ()))
    return (Key_pressed ());
  return (-1);
}
/* DESCRIPTION: This program returnes codes form open event queue with a  *
 *    relative device (mouse) and a string device (keyboard).  (-levent)  */
int
Key_pressed ()
{
  extern void Term_Change_Symbol ();
  int Symbol, X, Y, absX, absY, HowMuch, i, j, k, y, s[EV_STR_BUFSIZE + 1];
  EVENT *evp, ev;
  /* EV_STR_BUFSIZE = 8; defined in sys/event.h */

  evp = &ev;
  /* Events should be enqueued now */
  for (i = 0; i <= EV_STR_BUFSIZE; i++)
    s[i] = 0;
  HowMuch = ev_count ();
  for (i = 0, y = 0; i < HowMuch && i <= EV_STR_BUFSIZE; i++)
    {
      evp = ev_read ();
      if (EV_TAG (*evp) & T_STRING)
        {
          j = EV_BUFCNT (*evp);
          for (k = 0; j > 0; y++, j--)
            s[y] = EV_BUF (*evp)[k++];
        }
      else
        {
          ev_pop ();
          break;
        }
      ev_pop ();
    }
  if (EV_TAG (*evp) & T_STRING)
    {
      Symbol = s[0];
      switch (Symbol)
        {
        case 27:
          if (s[1] == 91)
            {
              Symbol = s[2];
              Symbol += 300;
            }
          break;
          /* vt100 */
        case 349:
        case 350:
        case 351:
          Symbol = s[1];
          if (Symbol > 47 && Symbol < 58)
            Term_Change_Symbol (&Symbol);
          break;
        }
      return (Symbol);
    }
  else /* if((EV_TAG(*evp) & T_REL_LOCATOR) | (EV_TAG(*evp) & T_BUTTON)) */
    {
      if (EV_TAG (*evp) & T_BUTTON)
        {
          switch ((int) EV_BUTTONS (*evp))
            {
            case BUTTON1: return (27);
            case BUTTON2:
            case BUTTON3: return (13);
            }
        }
      absX = X = EV_DY (*evp);
      absY = Y = EV_DX (*evp);
      if (X == 0 && Y == 0)
        return (-1);
      if (absX < 0)
        absX = (-1) * absX;
      if (absY < 0)
        absY = (-1) * absY;
      if (absX > absY)
        {
          if (X < 0)
            Symbol = 366;
          else
            Symbol = 365;
        }
      else
        {
          if (Y < 0)
            Symbol = 368;
          else
            Symbol = 367;
        }
      ev_flush ();
      return (Symbol);
    }
}
#else  /* _HAS_MOUSE */
int
evKey_pressed ()
{
  void Term_Change_Symbol ();
  int Symbol, TakeSymbol ();

  Symbol = TakeSymbol (T);
  switch (Symbol)
    {
#ifdef _M_TERMINFO
    case -2: /* Must be -2 because -1 is default in Key_pressed()! */
      return (27);
#endif /* _M_TERMINFO */
    case 27:
      Symbol = TakeSymbol (T);
      if (Symbol == 91)
        {
          Symbol = TakeSymbol (T);
          Symbol += 300;
          if (Symbol > 348 && Symbol < 352) /* vt100 */
            {
              Symbol = TakeSymbol (T);
              if (Symbol > 47 && Symbol < 58)
                Term_Change_Symbol (&Symbol);
              TakeSymbol (T);
            }
        }
      else if (Symbol == 27)
        {
          Symbol = evKey_pressed ();
          return (Symbol);
        }
      break;
#ifdef _M_TERMINFO
    case 91:
      Symbol = TakeSymbol (F);
      if (Symbol == -1)
        return (91);
      Symbol += 300;
      if (Symbol > 348 && Symbol < 352) /* vt100 */
        {
          Symbol = TakeSymbol (T);
          if (Symbol > 47 && Symbol < 58)
            Term_Change_Symbol (&Symbol);
          TakeSymbol (T);
        }
      break;
#endif /* _M_TERMINFO */
    }
  return (Symbol);
}
int
Key_pressed ()
{
#ifdef _M_TERMINFO
  void Term_Change_Symbol ();
  int Symbol, TakeSymbol ();

  Symbol = TakeSymbol (F);
  switch (Symbol)
    {
    case -2: /* Returns -1 if nothing is press, that is why -2. */
      return (27);
    case 27:
      Symbol = TakeSymbol (T);
      if (Symbol == 91)
        {
          Symbol = TakeSymbol (T);
          Symbol += 300;
          if (Symbol > 348 && Symbol < 352) /* vt100 */
            {
              Symbol = TakeSymbol (T);
              if (Symbol > 47 && Symbol < 58)
                Term_Change_Symbol (&Symbol);
              TakeSymbol (T);
            }
        }
      else if (Symbol == 27)
        {
          Symbol = evKey_pressed ();
          return (Symbol);
        }
      break;
    case 91:
      Symbol = TakeSymbol (F);
      if (Symbol == -1)
        return (91);
      Symbol += 300;
      if (Symbol > 348 && Symbol < 352) /* vt100 */
        {
          Symbol = TakeSymbol (T);
          if (Symbol > 47 && Symbol < 58)
            Term_Change_Symbol (&Symbol);
          TakeSymbol (T);
        }
      break;
    }
  return (Symbol);
#else
  return (-1);
#endif /* _M_TERMINFO */
}
int
TakeSymbol (WAIT)

Boolean WAIT;
{
  extern WINDOW *Wkur;
  char Symbol;

  if (WAIT == T)
    Symbol = wgetch (Wkur);
  else
    {
#ifdef _M_TERMINFO      
      nodelay (Wkur, TRUE);
      Symbol = wgetch (Wkur);
      nodelay (Wkur, FALSE);
#else
      Symbol = 0;
#endif /* _M_TERMINFO */
    }
#ifdef _M_TERMINFO      
  if (Symbol == 27 && vtterm == F)
    {
      nodelay (Wkur, TRUE);
      Symbol = wgetch (Wkur);
      nodelay (Wkur, FALSE);
      if (Symbol == -1)
        return (-2);
    }
#endif /* _M_TERMINFO */
  return ((int) Symbol);
}
#endif /* _HAS_MOUSE */
void
Term_Change_Symbol (Symbol)

int *Symbol;
{
  *Symbol += 400;
  switch (*Symbol)
    {
    case 455: /* f6 terminal  */
      *Symbol = 382;
      break; /* f6 konzole    */
    case 456: /* f7 terminal  */
      *Symbol = 383;
      break; /* f7 konzole    */
    case 457: /* f8 terminal  */
      *Symbol = 24;
      break; /* Ctrl-x           */
    case 448: /* f9 terminal  */
      *Symbol = 385;
      break; /* f9 konzole    */
    case 449: /* f17 terminal */
      *Symbol = 372;
      break; /* Home konzole  */
    case 450: /* f18 terminal */
      *Symbol = 370;
      break; /* End konzole   */
    case 451: /* f19 terminale */
      *Symbol = 373;
      break; /* PgUp na konzole  */
    case 452: /* f20 terminal */
      *Symbol = 371;
      break; /* PgDn konzole  */
    }
}
/******************************* END ***************************************/
