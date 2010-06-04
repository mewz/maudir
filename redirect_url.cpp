/*
 *  redirect_url.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/3/10.
 *
 */

#include "redirect_url.h"

using namespace std;

void RedirectURL::http_redirect_url_handler(struct evhttp_request *request, void *args){
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