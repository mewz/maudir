/*
 *  event_utils.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#include "event_utils.h"

void print_to_client(struct evhttp_request *request, const char *response_str){
	struct evbuffer *buff = evbuffer_new();
	evbuffer_add_printf(buff, "%s", response_str);
	struct evkeyvalq *output_headers = evhttp_request_get_output_headers(request);
	evhttp_add_header(output_headers, "Content-Type", "text/html; charset=UTF-8");
	evhttp_send_reply(request, 200, "OK", buff);
	evbuffer_free(buff);
}

void redirect_client(struct evhttp_request *request, const char *location){
	struct evbuffer *buff = evbuffer_new();
	struct evkeyvalq *output_headers = evhttp_request_get_output_headers(request);
	evhttp_add_header(output_headers, "Content-Type", "text/html; charset=UTF-8");
	evhttp_add_header(output_headers, "Location", location);
	evhttp_send_reply(request, HTTP_MOVEPERM, "", buff);
	evbuffer_free(buff);
}