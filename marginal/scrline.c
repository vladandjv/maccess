/**************************************************************************
 *  SCRLINE.C
 *  Drows a line on the screen.
 *	Author: Vladan Djordjevic
 **************************************************************************/
#include "screen.h"
/********************************************************/
void
    wLine(MyWindow, wUp, wLeft, wRight)

        WINDOW *MyWindow;
long long wUp, wLeft, wRight;
{
  char out[MAX_STRING_LENGTH];

  wattron(MyWindow, A_ALTCHARSET);
  mvwprintw(MyWindow, wUp, wLeft, "%c", ACS_LTEE);
  mvwprintw(MyWindow, wUp, wLeft + 1, "%s", repeat(out, ACS_HLINE, wRight - wLeft - 1));
  mvwprintw(MyWindow, wUp, wRight, "%c", ACS_RTEE);
  wattroff(MyWindow, A_ALTCHARSET);
  wnoutrefresh(MyWindow);
}
/********************************************************/
void
    wVertical_Line(MyWindow, wUp, wLeft, wDown)

        WINDOW *MyWindow;
long long wUp, wLeft, wDown;
{
  long long i;

  wattron(MyWindow, A_ALTCHARSET);
  mvwprintw(MyWindow, wUp, wLeft, "%c", ACS_TTEE);
  for (i = wUp + 1; i <= wDown; i++)
    mvwprintw(MyWindow, i, wLeft, "%c", ACS_VLINE);
  mvwprintw(MyWindow, wDown, wLeft, "%c", ACS_BTEE);
  wattroff(MyWindow, A_ALTCHARSET);
  wnoutrefresh(MyWindow);
}
/********************************************************/
