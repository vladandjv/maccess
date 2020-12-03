/* SCRSHOWSYMBOLS.C */
#include "scrdefs.h"
/**************************************************************************/
void
ShowSymbols ()
{
  unsigned int i;

  i = ((int) LINES / 4) - 4;
#ifdef _M_TERMINFO
  Frame (i - 1, 34, i + 12, 59);
  wattron (stdscr, A_STANDOUT | COLOR_PAIR (PAIR4));
#else
  wattron (stdscr, A_STANDOUT);
  Frame (i - 1, 34, i + 12, 59);
  settext ();
#endif /* _M_TERMINFO */
  mvprintw (i, 35, "                        ");
  i++;
  mvprintw (i, 35, " Help   =        Ctrl-i ");
  i++;
  mvprintw (i, 35, " Break  = ESC or Ctrl-x ");
  i++;
  mvprintw (i, 35, " Exit   =        Ctrl-k ");
  i++;
  mvprintw (i, 35, " Insert =        Ctrl-u ");
  i++;
  mvprintw (i, 35, " Delete =     BackSpace ");
  i++;
  mvprintw (i, 35, " Home   =        Ctrl-a ");
  i++;
  mvprintw (i, 35, " End    =     or Ctrl-e ");
  i++;
  mvprintw (i, 35, " PgUp   =     or Ctrl-b ");
  i++;
  mvprintw (i, 35, " PgDn   =     or Ctrl-f ");
  i++;
  mvprintw (i, 35, " List   =     or Ctrl-l ");
  i++;
  mvprintw (i, 35, "                        ");
#ifdef _M_TERMINFO
  wattroff (stdscr, A_STANDOUT | COLOR_PAIR (PAIR4));
  wnoutrefresh (stdscr);
#else
  wattroff (stdscr, A_STANDOUT);
  wrefresh (stdscr);
#endif /* _M_TERMINFO */
  message ("Press any key to exit HELP!");
}
/**************************************************************************/
