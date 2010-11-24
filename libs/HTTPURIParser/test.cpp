//#define DEBUG
#include "HTTPURIParser.h"
#include <string>
#include <cstdio>
using std::string;
int main(int argc, char *argv[]){

	char *uri[] = {"http://ccfit.nsu.ru/~sviridov",
		"http://proxy.nsu.ru:8080/cache",
		"http://kernel.org/",
		"cn.ru",
		"", NULL};

	//	std::string spaces_string("      hello world!!11                       ");
	//	HTTPURIParser::trim_spaces(spaces_string);
	//	cout << spaces_string << endl;

	for(char **i = uri; *i != NULL; ++i){
		fprintf(stderr, "processing %s\n", *i);
		ParsedURI *pu;
		if((pu = HTTPURIParser::parse(string(*i))) == NULL){
			fprintf(stderr, "NULL returned\n");
		} else {
			fprintf(stderr, "scheme: %s\nnetloc: %s\nport: %s (%d)\npath: %s\n", 
					pu->scheme.c_str(),
					pu->netloc.c_str(),
					pu->port_s.c_str(),
					pu->port_n,
					pu->path.c_str());
			delete pu;
		}
	}
	return 0;
}
