/*
 *  data_store.h
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#ifndef __DATA_STORE__H_
#define __DATA_STORE__H_

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <libmemcached/memcached.h>
#include <mysql++.h>
#include <iostream>
#include "base62.h"
#include "mau_config.h"

class DataStore{

public:
	static void init_memcached(void);	
	static void init_mysql(void);
	static void init_services(void);
	static const char* short_url_from_url(const char*);
	static const char* create_short_url_from_url(const char *);
	static const char* memcached_url_from_key(const char*);
	static const char* value_from_key(const char*);
	static const char* mysql_url_from_key(const char*);
};
#endif
