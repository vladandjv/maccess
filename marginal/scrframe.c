/**************************************************************************
 *  SCRFRAME.C
 *  Drows a frame on the screen
 *	Author: Vladan Djordjevic
 **************************************************************************/
#include "screen.h"
/********************************************************/
void
    wFrame(MyWindow, wUp, wLeft, wDown, wRight)

        WINDOW *MyWindow;
long long wUp, wLeft, wDown, wRight;
{
  long long i;
  char out[MasAchRow];

  wattron(MyWindow, A_ALTCHARSET);
  mvwprintw(MyWindow, wUp, wLeft, "%c", ACS_ULCORNER);
  mvwprintw(MyWindow, wUp, wLeft + 1, "%s", repeat(out, ACS_HLINE, wRight - wLeft - 1));
  mvwprintw(MyWindow, wUp, wRight, "%c", ACS_URCORNER);
  for (i = wUp + 1; i < wDown; i++)
  {
    mvwprintw(MyWindow, i, wLeft, "%c", ACS_VLINE);
    mvwprintw(MyWindow, i, wRight, "%c", ACS_VLINE);
  }
  mvwprintw(MyWindow, wDown, wLeft, "%c", ACS_LLCORNER);
  mvwprintw(MyWindow, wDown, wLeft + 1, "%s", repeat(out, ACS_HLINE, wRight - wLeft - 1));
  mvwprintw(MyWindow, wDown, wRight, "%c", ACS_LRCORNER);
  wattroff(MyWindow, A_ALTCHARSET);
  wnoutrefresh(MyWindow);
}
/********************************************************/
