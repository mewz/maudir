/*
 *  create_short_url.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#include "create_short_url.h"

using namespace std;

gboolean
CreateShortURL::parse_url (const char  *inUrl,  /* IN */
                           char       **outUrl) /* OUT */
{
	static gsize initialized = FALSE;
	gboolean ret = FALSE;
	char *downstr = NULL;
	char *scheme = NULL;
	char *host = NULL;
	char *rawPath = NULL;
	char *path = NULL;

	if (g_once_init_enter(&initialized)) {
		HTLibInit("CreateShortURL", "1.0");
		g_once_init_leave(&initialized, TRUE);
	}

	*outUrl = NULL;
	if (HTURL_isAbsolute(inUrl)) {
		if (!(downstr = g_utf8_strdown(inUrl, -1))) {
			goto invalid_encoding;
		}
		if (!(scheme = HTParse(downstr, "", PARSE_ACCESS))) {
			goto invalid_url;
		}
		if (strcmp(scheme, "http") != 0 && strcmp(scheme, "https") != 0) {
			goto invalid_scheme;
		}
		if (!(host = HTParse(downstr, "", PARSE_HOST))) {
			goto invalid_host;
		}
		if (!CreateShortURL::is_valid_host(host)) {
			goto invalid_host;
		}
		if (!(rawPath = HTParse(inUrl, "", PARSE_PATH))) {
			goto invalid_path;
		}
		if (!(path = CreateShortURL::parse_path(rawPath))) {
			goto invalid_path;
		}
		*outUrl = g_strdup_printf("%s://%s/%s", scheme, host, path);
		ret = TRUE;
	}
  invalid_encoding:
  invalid_url:
  invalid_scheme:
  invalid_host:
  invalid_path:
	g_free(downstr);
	g_free(scheme);
	g_free(host);
  	return ret;
}

char*
CreateShortURL::parse_path (const char *inPath) /* IN */
{
	gboolean insertPath = FALSE;
	GString *str;
	gchar *ret;
	int len;
	int i;

	g_return_val_if_fail(inPath != NULL, NULL);

	len = strlen(inPath);
	str = g_string_sized_new(len);
	for (i = 0; i < len; i++) {
		if (!insertPath) {
			if (inPath[i] != '/') {
				insertPath = TRUE;
				g_string_append_c(str, '/');
				g_string_append_c(str, inPath[i]);
			}
			continue;
		}
		g_string_append_c(str, inPath[i]);
	}
	ret = str->str;
	g_string_free(str, FALSE);
	return ret;
}

gboolean
CreateShortURL::is_valid_host (const char *host) /* IN */
{
	static gsize initialized = FALSE;
	static char *server_host;
	gboolean ret = false;
	char *host_lower;
	int i;

	if (g_once_init_enter(&initialized)) {
		server_host = g_utf8_strdown(MAU_SERVER_NAME, -1);
		g_once_init_leave(&initialized, TRUE);
	}

	if (!(host_lower = g_utf8_strdown(host, -1))) {
		goto invalid_encoding;
	}
	if ((g_strcmp0(server_host, host_lower) == 0) ||
	    (g_strcmp0(host_lower, "localhost") == 0)) {
		goto invalid_host;
	}
	//cheap way of parsing for a domain -
	//we do not support IP addresses, so make sure the host contains at least one alpha
	for (i = 0; host[i]; i++) {
		if (g_ascii_isalpha(host[i])) {
			ret = TRUE;
			goto success;
		}
	}
  invalid_encoding:
  invalid_host:
  success:
  	g_free(host_lower);
	return ret;
}

void
CreateShortURL::http_create_url_handler (struct evhttp_request *request,
                                         void                  *args)
{
	static const char create_query[] = "/create/?";
	const char *req_uri;
	char *url = NULL;
	char *url_id;
	char *real_url;

	g_return_if_fail(request != NULL);

	req_uri = evhttp_request_uri(request);
	if (strlen(req_uri) < sizeof(create_query)) {
		print_to_client(request, "invalid query param");
		return;
	}

	//remove the path and start of the query
	req_uri += sizeof(create_query);

	if (CreateShortURL::parse_url(req_uri, &url)) {
		if (!(url_id = DataStore::create_short_url_from_url(url))) {
			print_to_client(request, "invalid query param");
			g_free(url);
			return;
		}
		real_url = g_strdup_printf("%s%s", MAU_SERVER_URL, url_id);
		print_to_client(request, real_url);
		g_free(url_id);
		g_free(real_url);
		g_free(url);
	} else {
		print_to_client(request, "not a valid URL");
	}
}
