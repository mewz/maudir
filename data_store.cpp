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

void DataStore::init_memcached(){
	if(tcp_client == NULL){
		tcp_client = memcached_create(NULL);
		memcached_server_add(tcp_client, "127.0.0.1", 11211);
	}
}

void DataStore::init_mysql(){
	if(!conn.connected()){
		if(!conn.connect(DB, DBSERVER, DBUSER, DBPASSWORD, DBPORT)){
			fprintf(stderr, "Fatal error: Could not connect to MySQL server");
			exit(-1);
		}
	}
}

void DataStore::init_services(){
	DataStore::init_memcached();
	DataStore::init_mysql();
}

const char* DataStore::short_url_from_url(const char *url){
	DataStore::init_services();
	//must query MySQL - memcached only stores the shorturl=>url
	char *resstr = NULL;
	try{
		Query query = conn.query();
		query << "SELECT id from url_1 where url = " << quote << url;
		if(StoreQueryResult res = query.store()){
			if(res.num_rows() > 0){
				const char *t_url = res[0][0].c_str();
				u_int64_t id = atoll((char*)t_url);
				resstr = (char*)malloc(sizeof(id)*2);
				memset(resstr, '\0', sizeof(id)*2);
				sprintf(resstr, "%llu", (unsigned long long)id);
				return (const char*)resstr;
			}
		}
	}
	catch(const BadOption& err){
		std::cerr << err.what() << std::endl;
	}
	catch(const Exception& err){
		std::cerr << err.what() << std::endl;
	}
	
	return (const char*)resstr;
}

const char* DataStore::create_short_url_from_url(const char *url){
	DataStore::init_services();
	char *resset = (char*)DataStore::short_url_from_url(url);
	char encoded[12] = { 0 };
	if(resset == NULL){
		try{
			//insert into MySQL
			Query query = conn.query();
			query << "INSERT INTO url_1(url) values(" << mysqlpp::quote << url << ")";
			SimpleResult res = query.execute();
			base62_encode_uint64(res.insert_id(), encoded, sizeof(encoded));
			//insert into memcached
			memcached_set(tcp_client, resset, strlen(resset), url, strlen(url), (time_t)0, (uint32_t)0);
			resset = g_strdup(encoded);
		}
		catch(const BadOption& err){
			std::cerr << err.what() << std::endl;
		}
		catch(const Exception& err){
			std::cerr << err.what() << std::endl;
		}
	}
	else{
		base62_encode_uint64(atoll(resset), encoded, sizeof(encoded));
		free(resset);
		return g_strdup(encoded);
	}

	return (const char*)resset;
}

char* DataStore::memcached_url_from_key(const char* key){
	DataStore::init_services();
	memcached_return rc;
	size_t string_len;
	uint32_t flags;
	return memcached_get(tcp_client, key, strlen(key), &string_len, &flags, &rc);
}

char* DataStore::mysql_url_from_key(const char* key){
	DataStore::init_services();
	char* url_redir = NULL;
	guint64 store_id;

	if (!base62_decode_uint64(key, &store_id)) {
		return NULL;
	}

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
	return url_redir;
}

char* DataStore::value_from_key(const char* key){
	DataStore::init_services();
	char *value = DataStore::memcached_url_from_key(key);
	if(value == NULL){
		value = DataStore::mysql_url_from_key(key);
	}
	return value;
}
