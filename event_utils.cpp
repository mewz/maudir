/*
 *  event_utils.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 */

#include "event_utils.h"

/*
 *----------------------------------------------------------------------------
 *
 * print_to_client --
 *
 *     Outputs a successful response string to the http client
 *
 * Returns:
 *     None.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
void print_to_client(struct evhttp_request *request, /* IN */
					 const char *response_str) /* IN */
{
	struct evbuffer *buff = evbuffer_new();
	evbuffer_add_printf(buff, "%s", response_str);
	struct evkeyvalq *output_headers = evhttp_request_get_output_headers(request);
	evhttp_add_header(output_headers, "Content-Type", "text/html; charset=UTF-8");
	evhttp_send_reply(request, HTTP_OK, "OK", buff);
	evbuffer_free(buff);
}

/*
 *----------------------------------------------------------------------------
 *
 * redirect_client --
 *
 *     302 redirects the http client to a new location
 *
 * Returns:
 *     None.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
void redirect_client(struct evhttp_request *request, /* IN */
					 const char *location) /* IN */
{
	struct evbuffer *buff = evbuffer_new();
	struct evkeyvalq *output_headers = evhttp_request_get_output_headers(request);
	evhttp_add_header(output_headers, "Content-Type", "text/html; charset=UTF-8");
	evhttp_add_header(output_headers, "Location", location);
	evhttp_send_reply(request, HTTP_MOVEPERM, "", buff);
	evbuffer_free(buff);
}