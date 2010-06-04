#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_compat.h>

#include "create_short_url.h"
#include "redirect_url.h"

int main (int argc, char * const argv[]) {
	
	u_short port = 1025;
	const char *hostname = "127.0.0.1";

	struct event_base *base = event_base_new();
	struct evhttp *httpd = evhttp_new(base);
	
	evhttp_bind_socket(httpd, hostname, port);
	
	//set two callbacks. one to create short URL's and one to 302 them
	evhttp_set_cb(httpd, "/create/", CreateShortURL::http_create_url_handler, NULL);
	evhttp_set_gencb(httpd, RedirectURL::http_redirect_url_handler, NULL);
	event_base_dispatch(base);
	
	event_base_free(base);
	evhttp_free(httpd);
	
	return 0;
}
