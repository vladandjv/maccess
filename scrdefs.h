/**************************************************************************
 *      SCRDEFS.H
 *	Definitions for screen rutines.
 *	Author: Vladan Djordjevich.
 **************************************************************************/
#ifndef _SCRDEFS_H
#define _SCRDEFS_H

/* Use terminfo database to control terminal output. Must include
   libncurses.a libraries. */
#define _M_TERMINFO 

/* Use termcap database to control terminal output. Must include
   libtcap.a and libtermlib.a libraries. */
/* #undef _M_TERMCAP */

/* Default is set here. */
#if !defined( _M_TERMINFO ) && !defined( _M_TERMCAP )
#define _M_TERMINFO
#endif

#if defined( _M_TERMINFO ) && defined( _M_TERMCAP )
#ifdef _KR
#error Should #define only one of "_M_TERMCAP" or "_M_TERMINFO"
#else
#error Should #define only one of "_M_TERMCAP" or "_M_TERMINFO"
#endif /* _KR */
#endif

/* Use mouse drivers if found. Must include libevent.a library */
/* #undef _HAS_MOUSE */

#define _NOT_FCGI
#include "defs.h"
#include <curses.h> /* includes tinfo.h or tcap.h */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

extern Boolean vtterm; /* F - terminal is ansi, T - terminal is vt100 */

#ifdef _M_TERMINFO     /*********** color definitions *******************/
#define PAIR1 1
#define PAIR2 2
#define PAIR3 3
#define PAIR4 4
#define PAIR5 5
#define PAIR6 6
#endif /* _M_TERMINFO */
/*************************** pseudo functions (macros) *******************/
#if __cplusplus
extern "C" {
#endif /* __cplusplus */

#define Delete_Screen(a,b,c,d) wDelete_Screen(stdscr,a,b,c,d)
#define Frame(a,b,c,d) wFrame(stdscr,a,b,c,d)
#define Line(x,y,z)    wLine(stdscr,x,y,z)
#define Vertical_Line(x,y,z)  wVertical_Line(stdscr,x,y,z)
    extern WINDOW *Wsav;
#define savescreen()    (Wsav = save_on_screen())
#define restscreen()    rest_from_screen(Wsav)
    /***************************************************************************/
#ifdef _M_TERMINFO
#define settext(); ;
#define setgraf(); ;
#else
    /***************************** functions **********************************/
    void setgraf(), settext();
#endif /* _M_TERMINFO */
    /***************************** functions **********************************/
    void SwIns(), GetField(), wKutija(), Help(), GetSquare();
    void InitEkran(), TermEkran();
    void wFrame(), wLine(), wDelete_Screen(), wVertical_Line();
    void ShowSymbols(), GetDatum(), Setpr(), Resetpr(), message();
    void cpyoblik(), rest_from_screen();
#ifdef _M_TERMCAP
    void PrScr();
#endif /* _M_TERMCAP */
    WINDOW *save_on_screen();
    char *my_time(), *time_difference(), *vrmuredi();
    long long timesecond();
    int evKey_pressed(), Key_pressed(), Achoice(), Read(), Look();
    Boolean answer(), datprov(), datprpr();
    Boolean check_number(), check_number_empty(), check_character(), check_dot_empty(), check_dot();

#if __cplusplus
};
#endif /* __cplusplus */

#endif  /* _SCRDEFS_H */
/******************************* END **************************************/
