#ifndef VT100_H
#define VT100_H

/* VT escape sequences */
#define VT_ESC(seq) "\033[" seq "m"
#define VT_CLEAR() VT_ESC("0")
#define VT_STYLE(str,style) VT_ESC(style) str VT_CLEAR()
#define VT_BOLD(c)   "1;"c
#define VT_DEFAULT   "39"
#define VT_BLACK     "30"
#define VT_RED       "31"
#define VT_LIRED     "91"
#define VT_BGRED     "41"
#define VT_GREEN     "32"
#define VT_LIGREEN   "92"
#define VT_YELLOW    "33"
#define VT_LIYELLOW  "93"
#define VT_BLUE      "34"
#define VT_LIBLUE    "94"
#define VT_MAGENTA   "35"
#define VT_LIMAGENTA "95"
#define VT_CYAN      "36"
#define VT_LICYAN    "96"
#define VT_LIGRAY    "37"
#define VT_DKGRAY    "90"
#define VT_WHITE     "97"

#define VT100_RED VT_ESC(VT_RED)
#define VT100_YLW VT_ESC(VT_YELLOW)
#define VT100_RST VT_CLEAR()

#endif /*VT100_H*/
