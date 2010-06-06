/*
 *  log.h
 *  maudir
 *
 *  Created by Christian Hergert 6/6/2010.
 *
 */

#ifndef __LOG__H_
#define __LOG__H_

#include <glib.h>

#ifdef ENABLE_TRACE
#define TRACE(_f, ...)   Log::trace(G_STRFUNC, __LINE__, _f, ##__VA_ARGS__)
#define ENTRY            Log::trace(G_STRFUNC, __LINE__, "ENTRY: ")
#define EXIT             do { Log::trace(G_STRFUNC, __LINE__, " EXIT: "); return; } while (0)
#define RETURN(_r)       do { Log::trace(G_STRFUNC, __LINE__, " EXIT: "); return _r; } while (0)
#define GOTO(_l)         do { Log::trace(G_STRFUNC, __LINE__, " GOTO: %s ", #_l); goto _l; } while (0)
#else
#define TRACE(_f, ...)
#define ENTRY
#define EXIT return
#define RETURN(_r) return _r
#define GOTO(_l) goto _l
#endif

#define DEBUG(_f, ...)   Log::debug(_f, ##__VA_ARGS__)
#define WARNING(_f, ...) Log::warning(_f, ##__VA_ARGS__)
#define ERROR(_f, ...)   Log::error(_f, ##__VA_ARGS__)

class Log {

public:
	static void init   (void);	
	static void trace  (const char *func,
	                    int         lineno,
	                    const char *format, ...) G_GNUC_PRINTF(3, 4);
	static void debug  (const char *format, ...) G_GNUC_PRINTF(1, 2);
	static void warning(const char *format, ...) G_GNUC_PRINTF(1, 2);
	static void error  (const char *format, ...) G_GNUC_PRINTF(1, 2);
};
#endif
