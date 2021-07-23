/**************************************************************************
 *  SCREEN.H
 *	Definitions for screen rutines
 *	Author: Vladan Djordjevic
 **************************************************************************/
#ifndef _SCREEN_H
#define _SCREEN_H

/* Use terminfo database to control terminal output. Must include
   libncurses.a libraries. */
//#define _M_TERMINFO

#include "maccess.h"
#include "stringv.h"
#include <curses.h> /* includes tinfo.h or tcap.h */
#include <ctype.h>
#include <stdlib.h>

/*********** color definitions **********************************************/
#define PAIR1 1
#define PAIR2 2
#define PAIR3 3
#define PAIR4 4
#define PAIR5 5
#define PAIR6 6
/*************************** macros *****************************************/
#if __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define MasAchRow 2500

    typedef char *AchMatPtr[MasAchRow];
    extern Boolean Direct_Exit;
    extern WINDOW *Wsav;

#define Delete_Screen(a, b, c, d) wDelete_Screen(stdscr, a, b, c, d)
#define Frame(a, b, c, d) wFrame(stdscr, a, b, c, d)
#define Line(x, y, z) wLine(stdscr, x, y, z)
#define Vertical_Line(x, y, z) wVertical_Line(stdscr, x, y, z)
#define savescreen() (Wsav = save_on_screen())
#define restscreen() rest_from_screen(Wsav)

    /***************************** functions ************************************/
    void SwIns(), GetField(), wKutija(), Help(), GetSquare();
    void InitEkran(), TermEkran(), FreeGetFieldMemory();
    void wFrame(), wLine(), wDelete_Screen(), wVertical_Line();
    void ShowSymbols(), GetDatum(), Setpr(), Resetpr(), message();
    void cpyoblik(), rest_from_screen();
    WINDOW *save_on_screen();
    char *my_time(), *time_difference(), *vrmuredi();
    long long timesecond();
    long long evKey_pressed(), Key_pressed(), Achoice(), Read(), Look();
    Boolean answer(), check_number(), check_number_empty(), check_character();
    Boolean check_dot_empty(), check_dot();

#if __cplusplus
};
#endif /* __cplusplus */

#endif /* _SCREEN_H */
/******************************* END **************************************/
