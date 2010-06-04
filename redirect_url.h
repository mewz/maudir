/*
 *  redirect_url.h
 *  maudir
 *
 *  Created by Jason Hullinger on 6/3/10.
 *
 */

#ifndef __REDIRECT_URL_H_
#define __REDIRECT_URL_H_

#include <iostream>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_compat.h>
#include "event_utils.h"
#include "string_utils.h"
#include "data_store.h"

#ifndef SERVERHOST
#define SERVERHOST "192.168.2.58"
#endif
#ifndef SERVERURL
#define SERVERURL "http://192.168.2.58:1025/"
#endif

class RedirectURL{
public:
	static void http_redirect_url_handler(struct evhttp_request*, void*);
};


#endif