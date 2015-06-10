#include <iostream>
#include <list>
#include <string>
#include <pthread.h>
#include <sstream>

using namespace std;

static void *print(void *ptr){
	stringstream buffer;

	buffer << "Thread " << pthread_self() << ": " 
		<< static_cast<char*>(ptr) << endl;

	cout << buffer.str();
}

int main(int argc, char *argv[]){
	list<string> strs;

	cout << "Input strings you wish threads to print (end with empty string):" << endl;

	while(true){
		string str;
		getline(cin, str);

		if(str.length() == 0)
			break;

		strs.push_back(str);
	}

	int num_of_threads = strs.size();

	int i = 0;
	pthread_t tids[num_of_threads];

	for(list<string>::iterator k = strs.begin(); k != strs.end(); ++k){
		char *s = const_cast<char*> ( (*k).c_str() );
		pthread_create(&tids[i++], NULL, &print, s);
	}
	
	for(i = 0; i < num_of_threads; ++i)
		pthread_join(tids[i], NULL);

	return 0;
}
