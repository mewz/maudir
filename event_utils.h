/*
 *  event_utils.h
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#ifndef __EVENT_UTILS_H_
#define __EVENT_UTILS_H_

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/http_compat.h>

void print_to_client(struct evhttp_request *request, const char *response_str);
void redirect_client(struct evhttp_request *request, const char *location);

#endif