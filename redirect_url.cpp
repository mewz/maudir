/*
 *  redirect_url.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/3/10.
 *
 */

#include "redirect_url.h"

using namespace std;

/**
 * RedirectURL::http_redirect_url_handler:
 * @request: An incoming evhttp_request.
 * @args:
 *
 * Redirect url handler for libevent2.
 *
 * Returns: None.
 * Side effects: None.
 */
void
RedirectURL::http_redirect_url_handler (struct evhttp_request *request, /* IN */
                                        void                  *args)    /* IN */
{
	const char *req_uri;
	char *url;

	g_return_if_fail(request != NULL);

	req_uri = evhttp_request_uri(request);
	req_uri++;
	url = DataStore::value_from_key(req_uri);
	if (url != NULL) {
		redirect_client(request, url);
	} else {
		print_to_client(request, "invalid id");
	}
	free(url);
}
