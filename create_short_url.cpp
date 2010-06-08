/*
 *  create_short_url.cpp
 *  maudir
 *
 *  Created by Jason Hullinger on 6/2/10.
 */

#include "create_short_url.h"

using namespace std;


/*
 *----------------------------------------------------------------------------
 *
 * parse_url --
 *
 *     Parses a URL string. Checks to make sure the URL is absolute and that
 *		the scheme is either http or https.
 *
 *		Upon error, an empty string is returned and the err bool will be false.
 *		Otherwise the parsed URL will be returned as lower case host and scheme
 *		and case insensitive path and query. Will not return an anchor.
 *
 * Returns:
 *     The parsed URL if successful; otherwise an empty string.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
string 
CreateShortURL::parse_url(std::string urlin, /* IN */ 
								 bool *err) /* OUT */
{
	string parsedURL = "";
	HTLibInit("CreateShortURL", "1.0");
	bool parsedOK = false;
	if(HTURL_isAbsolute(urlin.c_str())){
		char* strtoparse = strtolower(urlin.c_str());

		char *c_scheme = HTParse(strtoparse, "", PARSE_ACCESS);
		string scheme(c_scheme);
		HT_FREE(c_scheme);
		
		if(scheme == "http" || scheme == "https"){
			char *c_host = HTParse(strtoparse, "", PARSE_HOST);
			string host(c_host);
			HT_FREE(c_host);
			
			if(CreateShortURL::is_valid_host(host)){
				parsedOK = true;
				char *c_path = HTParse(urlin.c_str(), "", PARSE_PATH);
				string t_path(c_path);
				HT_FREE(c_path);
				
				string path = CreateShortURL::parse_path(t_path);
				parsedURL = scheme + "://" + host + "/" + path;
			}
		}
		free(strtoparse);
	}
	
	HTLibTerminate();
	*err = parsedOK;
	return parsedURL;
}

/*
 *----------------------------------------------------------------------------
 *
 * parse_path --
 *
 *     Parses the path part of a URL string. Removes all leading '/'
 *
 *
 * Returns:
 *     The parsed path or an empty string
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
string 
CreateShortURL::parse_path(string pathin) /* IN */
{
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

/*
 *----------------------------------------------------------------------------
 *
 * is_valid_host --
 *
 *     Parses the host to validate that it's not an IP address nor localhost
 *
 * Returns:
 *     TRUE upon success; otherwise FALSE.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
bool 
CreateShortURL::is_valid_host(string host) /* IN */
{
	char *cserver_host = strtolower(MAU_SERVER_NAME);
	string server_host(cserver_host);
	free(cserver_host);
	//string server_host(strtolower(MAU_SERVER_NAME));
	
	char *chost_lower(strtolower(host.c_str()));
	string host_lower(chost_lower);
	free(chost_lower);
	//string host_lower(strtolower(host.c_str()));

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


/*
 *----------------------------------------------------------------------------
 *
 * http_create_url_handler --
 *
 *     The libevent http callback handler used in evhttp_set_gencb or 
 *		evhttp_set_cb. This will either query and display a short URL if it's
 *		previously been stored. Otherwise it will create and store a short URL.
 *
 * Returns:
 *		None.
 *
 * Side effects:
 *     None.
 *
 *----------------------------------------------------------------------------
 */
void 
CreateShortURL::http_create_url_handler(struct evhttp_request *request, /* IN */
											 void *args /* IN */){
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
		char* turl = (char*)malloc(url.length()+1);
		memset(turl, '\0', url.length()+1);
		strncpy(turl, url.c_str(), url.length());
		char *url_id = DataStore::create_short_url_from_url(turl);
		free(turl);
		string short_url = MAU_SERVER_URL;
		if(url_id != NULL){
			short_url += url_id;
		}
		delete url_id;
		//string output = "<a href='" + short_url + "'>" + short_url + "</a>";
		print_to_client(request, short_url.c_str());
	}
	else{
		print_to_client(request, "not a valid URL");
	}
}