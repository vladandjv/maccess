#include "scrdefs.h"
/********************************************************/
void
wLine (MyWindow, wUp, wLeft, wRight)

WINDOW *MyWindow;
int wUp, wLeft, wRight;
{
#ifdef  _M_TERMCAP
  char ACS_LTEE, ACS_HLINE, ACS_RTEE;

  if (vtterm == T)
    {
      ACS_LTEE = '\x74';
      ACS_HLINE = '\x71';
      ACS_RTEE = '\x75';
    }
  else
    {
      ACS_LTEE = '\xC3';
      ACS_HLINE = '\xC4';
      ACS_RTEE = '\xB4';
    }
#endif /* _M_TERMCAP */

#ifdef _M_TERMINFO  
  wattron (MyWindow, A_ALTCHARSET);
#endif /* _M_TERMINFO */
  mvwprintw (MyWindow, wUp, wLeft, "%c", ACS_LTEE);
  mvwprintw (MyWindow, wUp, wLeft + 1, "%s", ponovi (ACS_HLINE, wRight - wLeft - 1));
  mvwprintw (MyWindow, wUp, wRight, "%c", ACS_RTEE);
#ifdef _M_TERMINFO
  wattroff (MyWindow, A_ALTCHARSET);
  wnoutrefresh (MyWindow);
#endif /* _M_TERMINFO */
}
/********************************************************/
void
wVertical_Line (MyWindow, wUp, wLeft, wDown)

WINDOW *MyWindow;
int wUp, wLeft, wDown;
{
  int i;
#ifdef  _M_TERMCAP
  char ACS_TTEE, ACS_VLINE, ACS_BTEE;

  if (vtterm == T)
    {
      ACS_TTEE = '\x77';
      ACS_VLINE = '\x78';
      ACS_BTEE = '\x76';
    }
  else
    {
      ACS_TTEE = '\xC2';
      ACS_VLINE = '\xB3';
      ACS_BTEE = '\xC1';
    }
#endif /* _M_TERMCAP */

#ifdef _M_TERMINFO  
  wattron (MyWindow, A_ALTCHARSET);
#endif /* _M_TERMINFO */
  mvwprintw (MyWindow, wUp, wLeft, "%c", ACS_TTEE);
  for (i = wUp + 1; i <= wDown; i++)
    mvwprintw (MyWindow, i, wLeft, "%c", ACS_VLINE);
  mvwprintw (MyWindow, wDown, wLeft, "%c", ACS_BTEE);
#ifdef _M_TERMINFO
  wattroff (MyWindow, A_ALTCHARSET);
  wnoutrefresh (MyWindow);
#endif /* _M_TERMINFO */
}
/********************************************************/
