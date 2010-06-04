/*
 *  create_short_url.h
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */


#ifndef __CREATE_SHORT_URL_H_
#define __CREATE_SHORT_URL_H_

#include <iostream>
#include <string>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_compat.h>
#include <wwwconf.h>
#include <w3c-libwww/WWWLib.h>
#include "event_utils.h"
#include "string_utils.h"
#include "data_store.h"

#ifndef SERVERHOST
#define SERVERHOST "192.168.2.58"
#endif
#ifndef SERVERURL
#define SERVERURL "http://192.168.2.58:1025/"
#endif

class CreateShortURL{
public:
	static void http_create_url_handler(struct evhttp_request*, void*);
	static bool is_valid_host(std::string);
	static std::string parse_url(std::string, bool*);
	static std::string parse_path(std::string);
};

#endif
