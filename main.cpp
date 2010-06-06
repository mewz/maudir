#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_compat.h>

#include "create_short_url.h"
#include "redirect_url.h"
#include "mau_config.h"
#include "log.h"

int main (int argc, char * const argv[]) {
	Log::init();

	DEBUG("Initializing libevent.");
	struct event_base *base = event_base_new();
	struct evhttp *httpd = evhttp_new(base);
	//set two callbacks. one to create short URL's and one to 302 them
	evhttp_set_cb(httpd, "/create/", CreateShortURL::http_create_url_handler, NULL);
	evhttp_set_gencb(httpd, RedirectURL::http_redirect_url_handler, NULL);

	DEBUG("Binding libevent socket to %s:%d.", MAU_SERVER_NAME, MAU_LISTEN_PORT);
	evhttp_bind_socket(httpd, MAU_SERVER_NAME, MAU_LISTEN_PORT);

	DEBUG("libevent mainloop starting.");
	event_base_dispatch(base);

	event_base_free(base);
	evhttp_free(httpd);

	return 0;
}
