/*
 *  data_store.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#include "data_store.h"

using namespace mysqlpp;

static memcached_st *tcp_client = NULL;
static Connection conn;

/*
 *----------------------------------------------------------------------------
 *
 * init_memcached --
 *
 *     If the static memcached client is NULL, create an instance for a host 
 *		and port
 *
 * Returns:
 *     None.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
void DataStore::init_memcached()
{
	if(tcp_client == NULL){
		tcp_client = memcached_create(NULL);
		memcached_server_add(tcp_client, MEMCACHED_SERVER_HOST, MEMCACHED_SERVER_PORT);
	}
}

/*
 *----------------------------------------------------------------------------
 *
 * init_mysql --
 *
 *     If the static MySQL client is NULL, connect to a MySQL server
 *
 * Returns:
 *     None.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
void DataStore::init_mysql()
{
	if(!conn.connected()){
		if(!conn.connect(DB, DBSERVER, DBUSER, DBPASSWORD, DBPORT)){
			fprintf(stderr, "Fatal error: Could not connect to MySQL server");
			exit(-1);
		}
	}
}

/*
 *----------------------------------------------------------------------------
 *
 * init_services --
 *
 *	Initialize the memcached and MySQL clients
 *
 * Returns:
 *     None.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
void DataStore::init_services()
{
	DataStore::init_memcached();
	DataStore::init_mysql();
}

/*
 *----------------------------------------------------------------------------
 *
 * url_id_from_url --
 *
 *	Fetch the long long id for a stored URL and return it as a string
 *
 * Returns:
 *     A string containing the database ID of a stored URL. If the URL does not
 *		yet exist, return NULL.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
const char* DataStore::url_id_from_url(const char *url) /* IN */
{
	DataStore::init_services();
	//must query MySQL - memcached only stores the shorturl=>url
	try{
		Query query = conn.query();
		query << "SELECT id from url_1 where url = " << quote << url;
		if(StoreQueryResult res = query.store()){
			if(res.num_rows() > 0){
				const char *t_url = res[0][0].c_str();
				char *err = NULL;
				u_int64_t id = strtoll(t_url, &err, 10);
				if(err == NULL){
					fprintf(stderr, "fatal error. id returned from DB is not valid");
					exit(-1);
				}
				std::string resstr;
				std::ostringstream ss;
				ss << id;
				resstr = ss.str();
				return resstr.c_str();
			}
		}
	}
	catch(const BadOption& err){
		std::cerr << err.what() << std::endl;
	}
	catch(const Exception& err){
		std::cerr << err.what() << std::endl;
	}
	
	return NULL;
}

/*
 *----------------------------------------------------------------------------
 *
 * create_short_url_from_url --
 *
 *	Query the DB for the URL. If we have already stored the URL, return a 
 *	base62 encoded ID. 
 *
 *	If the query fails, insert the URL and base62 encode the resulting id. Store
 *	the result into memcached for lookups or redirect
 *
 * Returns:
 *     A string containing the base62 encoded database ID of a stored URL.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
const char* DataStore::create_short_url_from_url(const char *url) /* IN */
{
	DataStore::init_services();
	char *resset = (char*)DataStore::url_id_from_url(url);
	if(resset == NULL){
		try{
			//insert into MySQL
			char encoded[12]; //max base62 encoding size
			Query query = conn.query();
			query << "INSERT INTO url_1(url) values(" << mysqlpp::quote << url << ")";
			SimpleResult res = query.execute();
			base62_encode(res.insert_id(), encoded, sizeof(encoded));
			//insert into memcached
			memcached_set(tcp_client, encoded, strlen(encoded), url, strlen(url), (time_t)0, (uint32_t)0);
			return strdup(encoded);
		}
		catch(const BadOption& err){
			std::cerr << err.what() << std::endl;
		}
		catch(const Exception& err){
			std::cerr << err.what() << std::endl;
		}
	}
	else{
		char encoded[12];
		if (base62_encode(atoll(resset), encoded, sizeof(encoded))) {
			return strdup(encoded);
		}
		return NULL;
	}
		
	return (const char*)resset;
}

/*
 *----------------------------------------------------------------------------
 *
 * memcached_url_from_key --
 *
 *	Query memcached for a URL containing the associated key
 *
 * Returns:
 *     A string containing the URL or NULL upon failure
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
const char* DataStore::memcached_url_from_key(const char* key) /* IN */
{
	DataStore::init_services();
	memcached_return rc;
	size_t string_len;
	uint32_t flags;
	return memcached_get(tcp_client, key, strlen(key), &string_len, &flags, &rc);
}

/*
 *----------------------------------------------------------------------------
 *
 * memcached_url_from_key --
 *
 *	Query MySQL for a URL containing the ID of the base62 decoded key
 *
 * Returns:
 *     A string containing the URL or NULL upon failure
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
const char* DataStore::mysql_url_from_key(const char* key) /* IN */
{
	DataStore::init_services();
	char* url_redir = NULL;
	uint64_t store_id = base62_decode((char*)key);
	Query query = conn.query();
	query << "SELECT url from url_1 where id = " << store_id;
	if(StoreQueryResult res = query.store()){
		if(res.num_rows() > 0){
			char *t_url = (char*)res[0][0].c_str();
			url_redir = (char*)malloc(sizeof(char)*strlen(t_url)+1);
			memset(url_redir, '\0', sizeof(char)*strlen(t_url)+1);
			strncpy(url_redir, (const char*)t_url, sizeof(char)*strlen(t_url)+1);
			//add to memcached
			memcached_set(tcp_client, key, strlen(key), url_redir, strlen(url_redir), (time_t)0, (uint32_t)0);
		}
	}
	return (const char*)url_redir;
}

/*
 *----------------------------------------------------------------------------
 *
 * value_from_key --
 *
 *	Query memcached for a URL containing the associated key. If memcached fails
 *	query MySQL for a URL containing the associated key.
 *
 * Returns:
 *     A string containing the URL or NULL upon failure
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
const char* DataStore::value_from_key(const char* key) /* IN */
{
	DataStore::init_services();
	const char *value = DataStore::memcached_url_from_key(key);
	if(value == NULL){
		value = DataStore::mysql_url_from_key(key);
	}
	return value;
}


