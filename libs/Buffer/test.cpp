#include "Buffer.h"
#include "VectorBuffer.h"
#include <cstdio>
int main(int argc, char *argv[]){
	const char *s1 = "abc";
	const char *s2 = "qwe";
	Buffer *b1 = new VectorBuffer(s1);
b1->append(s1);
b1->append(s2);

	printf("%s", b1->buf());
	
	delete b1;
	return 0;
}
