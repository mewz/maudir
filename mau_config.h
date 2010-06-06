/*
 *  mau_config.h
 *  maudir
 *
 *  Created by Jason Hullinger on 6/3/10.
 */

#ifndef MAU_SERVER_INFO
#define MAU_SERVER_INFO
#define MAU_SERVER_NAME "127.0.0.1"
#define MAU_LISTEN_PORT 1025
#define MAU_SERVER_URL "http://127.0.0.1:1025/"
#endif

#ifndef DB_SERVER_INFO
#define DB_SERVER_INFO
#define DB "urls"
#define DBSERVER "localhost"
#define DBUSER "user"
#define DBPASSWORD "pass1"
#define DBPORT 3006
#endif

#ifndef MEMCACHED_SERVER_INFO
#define MEMCACHED_SERVER_INFO
#define MEMCACHED_SERVER_HOST "127.0.0.1"
#define MEMCACHED_SERVER_PORT 11211
#endif
