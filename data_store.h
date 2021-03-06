/*
 *  data_store.h
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 */

#ifndef __DATA_STORE__H_
#define __DATA_STORE__H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <libmemcached/memcached.h>
#include <mysql++.h>
#include <iostream>
#include "string_utils.h"
#include "base62.h"
#include "mau_config.h"

class DataStore{

public:
	static void		init_memcached(void);	
	static void		init_mysql(void);
	static void		init_services(void);
	static char*		url_id_from_url(char *url); /* IN */
	static char*		create_short_url_from_url(char *url); /* IN */
	static char*		memcached_url_from_key(char *key); /* IN */
	static char*		mysql_url_from_key(char *key); /* IN */
	static char*		value_from_key(char *key); /* IN */
};
#endif
