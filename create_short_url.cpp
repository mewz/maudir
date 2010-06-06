/*
 *  create_short_url.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 *
 */

#include "create_short_url.h"

using namespace std;

string CreateShortURL::parse_url(std::string urlin, bool *err){
	string parsedURL = "";
	HTLibInit("CreateShortURL", "1.0");
	bool parsedOK = false;
	if(HTURL_isAbsolute(urlin.c_str())){
		char* strtoparse = g_utf8_strdown(urlin.c_str(), -1);
		string scheme = HTParse(strtoparse, "", PARSE_ACCESS);
		if(scheme == "http" || scheme == "https"){
			string host = HTParse(strtoparse, "", PARSE_HOST);
			if(CreateShortURL::is_valid_host(host)){
				parsedOK = true;
				string path = CreateShortURL::parse_path(HTParse(urlin.c_str(), "", PARSE_PATH));
				parsedURL = scheme + "://" + host + "/" + path;
			}
		}
		g_free(strtoparse);
	}
	
	HTLibTerminate();
	*err = parsedOK;
	return parsedURL;
}

string CreateShortURL::parse_path(string pathin){
	string path = "";
	//remove all leading '/' - only 1 is needed
	bool insertPath = false;
	for(size_t i = 0; i < pathin.length(); i ++){
		if(!insertPath){
			if(pathin.at(i) != '/'){
				insertPath = true;
				path.append(1, pathin.at(i));
			}
			continue;
		}
		path.append(1, pathin.at(i));
	}
	return path;
}

bool CreateShortURL::is_valid_host(string host){
	char *server_host = g_utf8_strdown(MAU_SERVER_NAME, -1); // make const, do once at startup
	char *host_lower = g_utf8_strdown(host.c_str(), -1);
	bool ret = false;
	size_t i;

	if ((g_strcmp0(server_host, host_lower) == 0) ||
	    (g_strcmp0(host_lower, "localhost") == 0)) {
		goto cleanup;
	}
	//cheap way of parsing for a domain -
	//we do not support IP addresses, so make sure the host contains at least one alpha
	for (i = 0; i < host.length(); i++) {
		if (g_ascii_isalpha(host.at(i))) {
			ret = true;
			goto cleanup;
		}
	}
  cleanup:
  	g_free(server_host);
  	g_free(host_lower);
	return ret;
}

void CreateShortURL::http_create_url_handler(struct evhttp_request *request, void *args){
	string create_query("/create/?");
	string req_uri(evhttp_request_uri(request));
	if(req_uri.length() < create_query.length()){
		print_to_client(request, "invalid query param");
		return;
	}
	//remove the path and start of the query
	req_uri = req_uri.substr(create_query.length(), req_uri.length());
	
	bool noErr;
	string url = CreateShortURL::parse_url(req_uri, &noErr);
	if(noErr){
		const char* url_id = DataStore::create_short_url_from_url(url.c_str());
		string short_url = MAU_SERVER_URL;
		if(url_id != NULL){
			short_url += url_id;
		}
		//string output = "<a href='" + short_url + "'>" + short_url + "</a>";
		print_to_client(request, short_url.c_str());
	}
	else{
		print_to_client(request, "not a valid URL");
	}
}
