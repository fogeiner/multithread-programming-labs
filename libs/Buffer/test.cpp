#include "Buffer.h"
#include "VectorBuffer.h"
#include <cstdio>
int main(int argc, char *argv[]){
	const char s1[] = "abc";
	const char s2[] = "qwe";
	Buffer *b1 = new VectorBuffer(s1, sizeof(s1) - 1);
	Buffer *b2 = new VectorBuffer(s2, sizeof(s2) - 1);

	(*b2) += b1;

	printf("%s", b2->buf());
	
	delete b1;
	delete b2;
	return 0;
}
