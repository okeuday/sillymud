/*
 * File: VT100C.H
 * Usage: VT100 code definitions
 * Copyright ()) 1993 by Dan Brumleve and the creators of DikuMUD.
 *
 * Part of:
 *   /\    ____                                     __     ____   |\   |
 *  /__\   |___|   /\  /\    /\    __  ___  +--\   |  \   /    \  | \  |
 * /    \  |   \  /  \/  \  /--\  /    [_   |   \  |   |  |    |  |  \ |
 * \    /  |   /  \      /  \  /  \_]  [__  |___/  |__/   \____/  |   \|
 *
 * Unauthorized distribution of this source code is punishable by 
 * eternal damnation to the hell pits of Suk-krath.  Have a nice day.
 *
 * Used with permission for SillyMUD
 */
 
 
 
#define UNKNOWN       0
#define VT100         1
 
#define VT_INITSEQ    "\033[1;24r"
#define VT_CURSPOS    "\033[%d;%dH" 
#define VT_CURSRIG    "\033[%dC"   
#define VT_CURSLEF    "\033[%dD"    
#define VT_HOMECLR    "\033[2J\033[0;0H"
#define VT_CTEOTCR    "\033[K"     
#define VT_CLENSEQ    "\033[r\033[2J" 
#define VT_INDUPSC    "\033M"
#define VT_INDDOSC    "\033D"
#define VT_SETSCRL    "\033[%d;24r"
#define VT_INVERTT    "\033[0;1;7m"
#define VT_BOLDTEX    "\033[0;1m"
#define VT_NORMALT    "\033[0m"
#define VT_MARGSET    "\033[%d;%dr"
#define VT_CURSAVE    "\0337"
#define VT_CURREST    "\0338"
#define ANSI_BLINK    "\033[5m"             /* blinking text */
 
 
/* Infobar stuffage added for SillyMUD */
#define INFO_HP            1
#define INFO_MANA          2
#define INFO_MOVE          4
#define INFO_EXP           8
#define INFO_GOLD          16

