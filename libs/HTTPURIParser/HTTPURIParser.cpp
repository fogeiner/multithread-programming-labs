#include "HTTPURIParser.h"

#ifdef DEBUG
#include <cstdio>
#endif


const std::string HTTPURIParser::http_scheme = "http";
const std::string HTTPURIParser::scheme_delimiter = "://";

void HTTPURIParser::trim_spaces(std::string &str) {
	int left = 0, right = str.size(), size = str.size();
	for(; left <= size; ++left){
		if(!isspace(str[left]))
			break;
	}

	for(; right != -1; --right){
		if(!isspace(str[right]))
			break;
	}

	str = str.substr(left, right);
}


ParsedURI *HTTPURIParser::parse(std::string uri){
	std::string scheme;
	std::string port;
	std::string netloc;
	std::string path;
	trim_spaces(uri);

	int index;
	unsigned short port_n = 0;
	// <scheme>://<netloc>[:<port>]/<path>

	// looking for ://
	index = uri.find(scheme_delimiter);

	if(index == -1) return NULL;
#ifdef DEBUG
	fprintf(stderr, "found scheme_delimeter at position %d\n", index);
#endif
	if(uri.substr(0, index) != http_scheme) return NULL;
#ifdef DEBUG
	fprintf(stderr, "http scheme detected\n");
#endif
	scheme = http_scheme;
	uri = uri.substr(index + scheme_delimiter.length());
#ifdef DEBUG
	fprintf(stderr, "string after removing scheme %s\n", uri.c_str());
#endif

	index = uri.find('/');
	if (index == -1) {
		path = '/';
	} else {
		path = uri.substr(index);
		uri = uri.substr(0, index);
	}
#ifdef DEBUG
	fprintf(stderr, "path is %s\n", path.c_str());
	fprintf(stderr, "remaining part of uri is %s\n", uri.c_str());
#endif

	index = uri.find(':');
	if (index == -1){
		netloc = uri;
	} else {
		char *endptr;
		netloc = uri.substr(0, index);
		port = uri.substr(index + 1);

		port_n = strtol(port.c_str(), &endptr, 10);
#ifdef DEBUG
		fprintf(stderr, "port_n is %d\n", port_n);
		fprintf(stderr, "endptr is %p\n", endptr);
#endif

		if(*endptr != '\0' || port_n < 0 || port_n > 65535){
			return NULL;
		}
	}



#ifdef DEBUG
	fprintf(stderr, "netloc is %s\n", netloc.c_str());
	fprintf(stderr, "port is %s\n", port.c_str());
#endif

	return new ParsedURI(scheme, netloc, port, path, port_n);

}

