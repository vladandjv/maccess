/**************************************************************************
 * SCRSHOWSYMBOLS.C 
 * Routines for string manipulations 
 * Author: Vladan Djordjevic
 **************************************************************************/

#include "screen.h"
/**************************************************************************/
void ShowSymbols()
{
  unsigned long long i;

  i = ((long long)LINES / 4) - 4;
  Frame(i - 1, 34, i + 12, 59);
  wattron(stdscr, A_STANDOUT | COLOR_PAIR(PAIR4));
  mvprintw(i, 35, "                        ");
  i++;
  mvprintw(i, 35, " Help   =        Ctrl-i ");
  i++;
  mvprintw(i, 35, " Break  = ESC or Ctrl-x ");
  i++;
  mvprintw(i, 35, " Exit   =        Ctrl-k ");
  i++;
  mvprintw(i, 35, " Insert =        Ctrl-u ");
  i++;
  mvprintw(i, 35, " Delete =     BackSpace ");
  i++;
  mvprintw(i, 35, " Home   =        Ctrl-a ");
  i++;
  mvprintw(i, 35, " End    =        Ctrl-e ");
  i++;
  mvprintw(i, 35, " PgUp   =        Ctrl-b ");
  i++;
  mvprintw(i, 35, " PgDn   =        Ctrl-f ");
  i++;
  mvprintw(i, 35, " List   =        Ctrl-l ");
  i++;
  mvprintw(i, 35, "                        ");
  wattroff(stdscr, A_STANDOUT | COLOR_PAIR(PAIR4));
  wnoutrefresh(stdscr);
  message("Press any key to exit HELP!");
}
/**************************************************************************/
