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
		const char* strtoparse = strtolower(urlin.c_str());
		string scheme = HTParse(strtoparse, "", PARSE_ACCESS);
		if(scheme == "http" || scheme == "https"){
			string host = HTParse(strtoparse, "", PARSE_HOST);
			if(CreateShortURL::is_valid_host(host)){
				parsedOK = true;
				string path = CreateShortURL::parse_path(HTParse(urlin.c_str(), "", PARSE_PATH));
				parsedURL = scheme + "://" + host + "/" + path;
			}
		}
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
	string server_host(strtolower(MAU_SERVER_NAME));
	string host_lower(strtolower(host.c_str()));
	string localhost("localhost");
	if(server_host == host_lower || host_lower == localhost){
		return false;
	}
	//cheap way of parsing for a domain -
	//we do not support IP addresses, so make sure the host contains at least one alpha
	for(size_t i = 0; i < host.length(); i ++){
		char c = host.at(i);
		if(c > 57){
			return true;
		}
	}
	return false;
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