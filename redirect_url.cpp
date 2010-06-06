/*
 *  redirect_url.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/3/10.
 *
 */

#include "redirect_url.h"

using namespace std;

/*
 *----------------------------------------------------------------------------
 *
 * http_create_url_handler --
 *
 *     The libevent http callback handler used in evhttp_set_gencb or 
 *		evhttp_set_cb. This will query the memcached for the base62 encoded key.
 *		Upon failure to find a URL from a key, it will query the DB.
 *
 *		302 redirect upon success. Print an error if the ID is invalid
 *
 * Returns:
 *		None.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
void 
RedirectURL::http_redirect_url_handler(struct evhttp_request *request, void *args){
	const char* req_uri = evhttp_request_uri(request);
	req_uri++;
	const char* url = DataStore::value_from_key(req_uri);
	if(url != NULL){
		redirect_client(request, url);
	}
	else{
		print_to_client(request, "invalid id");
	}
}