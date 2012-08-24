#include <stdio.h>

void p(int n) { 
	
	printf("%d\n", n); 
}

void foo(int n) {

	int i;

	if (n) {

		p(0);

		for (i=0; i < 3; i++) {
			p(1);
		}

		foo(0);

	} else {
		p(2);
	}
}

int main(int argc, char ** argv) { foo(1); return 0; }
