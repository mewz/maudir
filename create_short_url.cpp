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

bool
CreateShortURL::is_valid_host(string host)
{
	char *server_host = g_utf8_strdown(MAU_SERVER_NAME, -1); // make const, do once at startup
	char *host_lower = g_utf8_strdown(host.c_str(), -1);
	bool ret = false;
	size_t i;

	if ((g_strcmp0(server_host, host_lower) == 0) ||
	    (g_strcmp0(host_lower, "localhost") == 0)) {
		goto cleanup;
	}
	//cheap way of parsing for a domain -
	//we do not support IP addresses, so make sure the host contains at least one alpha
	for (i = 0; i < host.length(); i++) {
		if (g_ascii_isalpha(host.at(i))) {
			ret = true;
			goto cleanup;
		}
	}
  cleanup:
  	g_free(server_host);
  	g_free(host_lower);
	return ret;
}

void CreateShortURL::http_create_url_handler(struct evhttp_request *request, void *args){
	string create_query("/create/?");
	string req_uri(evhttp_request_uri(request));
	if(req_uri.length() < create_query.length()){
		print_to_client(request, "invalid query param");
		return;
	}
	//remove the path and start of the query
	req_uri = req_uri.substr(create_query.length(), req_uri.length());
	
	char *url;

	if (CreateShortURL::parse_url(req_uri.c_str(), &url)) {
		const char* url_id = DataStore::create_short_url_from_url(url);
		string short_url = MAU_SERVER_URL;
		if(url_id != NULL){
			short_url += url_id;
		}
		//string output = "<a href='" + short_url + "'>" + short_url + "</a>";
		print_to_client(request, short_url.c_str());
		g_free(url);
	}
	else{
		print_to_client(request, "not a valid URL");
	}
}
