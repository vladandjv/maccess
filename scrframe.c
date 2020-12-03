/* SCRFRAME.C */
#include "scrdefs.h"
/********************************************************/
void
wFrame (MyWindow, wUp, wLeft, wDown, wRight)

WINDOW *MyWindow;
int wUp, wLeft, wDown, wRight;
{
  int i;
#ifdef  _M_TERMCAP
  char ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER, ACS_HLINE, ACS_VLINE;

  if (vtterm == T)
    {
      ACS_ULCORNER = '\x6C';
      ACS_URCORNER = '\x6B';
      ACS_LLCORNER = '\x6D';
      ACS_LRCORNER = '\x6A';
      ACS_HLINE = '\x71';
      ACS_VLINE = '\x78';
    }
  else
    {
      ACS_ULCORNER = '\xDA';
      ACS_URCORNER = '\xBF';
      ACS_LLCORNER = '\xC0';
      ACS_LRCORNER = '\xD9';
      ACS_HLINE = '\xC4';
      ACS_VLINE = '\xB3';
    }
#endif /* _M_TERMCAP */

#ifdef _M_TERMINFO  
  wattron (MyWindow, A_ALTCHARSET);
#endif /* _M_TERMINFO */
  mvwprintw (MyWindow, wUp, wLeft, "%c", ACS_ULCORNER);
  mvwprintw (MyWindow, wUp, wLeft + 1, "%s", ponovi (ACS_HLINE, wRight - wLeft - 1));
  mvwprintw (MyWindow, wUp, wRight, "%c", ACS_URCORNER);
  for (i = wUp + 1; i < wDown; i++)
    {
      mvwprintw (MyWindow, i, wLeft, "%c", ACS_VLINE);
      mvwprintw (MyWindow, i, wRight, "%c", ACS_VLINE);
    }
  mvwprintw (MyWindow, wDown, wLeft, "%c", ACS_LLCORNER);
  mvwprintw (MyWindow, wDown, wLeft + 1, "%s", ponovi (ACS_HLINE, wRight - wLeft - 1));
  mvwprintw (MyWindow, wDown, wRight, "%c", ACS_LRCORNER);
#ifdef _M_TERMINFO
  wattroff (MyWindow, A_ALTCHARSET);
  wnoutrefresh (MyWindow);
#else
  if (vtterm == T)
    wrefresh (MyWindow);
#endif /* _M_TERMINFO */
}
/********************************************************/
