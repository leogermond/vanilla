#ifndef VANILLA_LOG_H
#define VANILLA_LOG_H
#include <stdio.h>

#include <config.h>

extern char *log_name;
extern int log_verbose;

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

/* Macros voodoo */
#define EXTRACT_FMT(f,...) f
#define EXTRACT_ARGS(f,...) ,##__VA_ARGS__
#define CFMT_CAT_2(a, b) EXTRACT_FMT a EXTRACT_FMT b EXTRACT_ARGS a EXTRACT_ARGS b
#define CFMT_CAT_3(a, b, c) EXTRACT_FMT a EXTRACT_FMT b EXTRACT_FMT c EXTRACT_ARGS a EXTRACT_ARGS b EXTRACT_ARGS c

/* Basic logging facilities */
#define log(d,color,id,f,...) VANILLA_DPRINTF(d, VT_STYLE(id, color)" %s: "f"\r\n", log_name, ##__VA_ARGS__)
#define vlog(...) if(log_verbose) log(__VA_ARGS__)
#define CFMT_CAT_LOC(...) CFMT_CAT_2((__VA_ARGS__), ("\t[%s:%d]", __FILE__, __LINE__))

/* User logging functions */
#define err(...)    log(2, VT_BOLD(VT_RED), "e", __VA_ARGS__)
#define warn(...)   log(1, VT_LIYELLOW,     "w", __VA_ARGS__)
#define info(...)   log(1, VT_GREEN,        "i", __VA_ARGS__)
#define dbg(...)   vlog(1, VT_LIBLUE,       "d", __VA_ARGS__)
#define trace(...) vlog(1, VT_BLUE,         "t", __VA_ARGS__)

#define err_loc(...)   err(  CFMT_CAT_LOC(__VA_ARGS__))
#define warn_loc(...)  warn( CFMT_CAT_LOC(__VA_ARGS__))
#define info_loc(...)  info( CFMT_CAT_LOC(__VA_ARGS__))
#define dbg_loc(...)   dbg(  CFMT_CAT_LOC(__VA_ARGS__))
#define trace_loc(...) trace(CFMT_CAT_LOC(__VA_ARGS__))

/* Assertions */
#define assert_failed_noloc(...) log(2, VT_BGRED, "a", __VA_ARGS__)
#define assert_failed(...) assert_failed_noloc(CFMT_CAT_LOC(__VA_ARGS__))
#define ASSERT_MSG(x,...) if(!(x)) assert_failed(__VA_ARGS__)
#define ASSERT(x) ASSERT_MSG(x, "expression \"" #x "\" is false")
#define ASSERT_OR_DIE(x)\
	do if(!(x)) {\
		assert_failed("expression \""#x"\" is false, die @%s()", __FUNCTION__);\
		goto die;\
	} while(0)

/* Trace/debug */
#define logft_var(f,t,v) trace_loc(#v " = " f, (t)v)

#define log_int(i) logft_var("%lld", long long, i)
#define log_hex(i) logft_var("0x%llx", long long, i)
#define log_float(f) logft_var("%lf", double, f)
#define log_str(s) logft_var("\"%s\"", char *, s)

#define log_enter(...) trace(CFMT_CAT_3(\
			(VT_STYLE(">", VT_LIGREEN) " %s(", __FUNCTION__), (__VA_ARGS__), (")")))

#define log_exit(...) trace(CFMT_CAT_2((VT_STYLE("<", VT_LIRED) " %s(): ", __FUNCTION__), (__VA_ARGS__)))

#endif /* VANILLA_LOG_H */
