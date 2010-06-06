/*
 *  log.cpp
 *  maudir
 *
 *  Created by Christian Hergert 6/6/2010
 *
 */

#include <string.h>
#include <time.h>

#ifdef __linux__
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif /* __linux__ */

#include "log.h"

G_LOCK_DEFINE(channels);
static GPtrArray *channels     = NULL;
static char       hostname[64] = { 0 };

static const char*
log_level_str (GLogLevelFlags level)
{
	#define CASE_LEVEL_STR(_n) case G_LOG_LEVEL_##_n: return #_n
	switch ((int)level) {
	CASE_LEVEL_STR(DEBUG);
	CASE_LEVEL_STR(WARNING);
	CASE_LEVEL_STR(ERROR);
	default:
		return "MESSAGE";
	}
	#undef CASE_LEVEL_STR
}

static void
log_handler (const char     *log_domain, /* IN */
             GLogLevelFlags  flags,      /* IN */
             const char     *message,    /* IN */
             gpointer        user_data)  /* IN */
{
	GIOChannel *channel;
	char *formatted;
	char ftime[32];
	time_t t;
	struct timespec ts;
	struct tm tt;
	gsize i;

	clock_gettime(CLOCK_REALTIME, &ts);
	t = (time_t)ts.tv_sec;
	tt = *localtime(&t);
	strftime(ftime, sizeof(ftime), "%Y/%m/%d %X", &tt);
	formatted = g_strdup_printf("%s %s: %s: %s\n",
	                            ftime, hostname, log_level_str(flags),
	                            message);
	{
		G_LOCK(channels);
		for (i = 0; i < channels->len; i++) {
			channel = (GIOChannel *)g_ptr_array_index(channels, i);
			g_io_channel_write_chars(channel, formatted, -1, NULL, NULL);
			g_io_channel_flush(channel, NULL);
		}
		G_UNLOCK(channels);
	}
	g_free(formatted);
}

void
Log::init (void)
{
	static gsize initialized = FALSE;

	if (g_once_init_enter(&initialized)) {
#ifdef __linux__
		struct utsname u;
		uname(&u);
		memcpy(hostname, u.nodename, sizeof(hostname));
#else
#ifdef __APPLE__
		gethostname(hostname, sizeof (hostname));
#else
#error "Target platform not supported"
#endif /* __APPLE__ */
#endif /* __linux__ */

		channels = g_ptr_array_new();
		g_ptr_array_add(channels, g_io_channel_unix_new(0));
		g_log_set_default_handler(log_handler, NULL);
		g_once_init_leave(&initialized, TRUE);
	}

	DEBUG("Logging subsystem initialized.");
}

void
Log::trace (const char *func,        /* IN */
            int         lineno,      /* IN */
            const char *format, ...) /* IN */
{
	GString *str;
	va_list args;

	str = g_string_new(NULL);
	va_start(args, format);
	g_string_append_vprintf(str, format, args);
	va_end(args);
	g_string_append_printf(str, " %s:%d", func, lineno);
	g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "%s", str->str);
	g_string_free(str, TRUE);
}

void
Log::debug (const char *format, ...) /* IN */
{
	va_list args;

	va_start(args, format);
	g_logv(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, format, args);
	va_end(args);
}

void
Log::warning (const char *format, ...) /* IN */
{
	va_list args;

	va_start(args, format);
	g_logv(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, format, args);
	va_end(args);
}

void
Log::error (const char *format, ...) /* IN */
{
	va_list args;

	va_start(args, format);
	g_logv(G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, format, args);
	va_end(args);
}
