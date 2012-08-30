#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer;

void testJmp(int par, ...) {

	if (par) 
		longjmp(jump_buffer, 1);

	printf("par is zero!\n");
}

int main(int argc, char ** argv) {

	if (!setjmp(jump_buffer)) {

		testJmp(1);
		printf("this message will not be printed!\n");
	
	} else {

		testJmp(0, 25, 100, 386, 4096);
	}	

	return 0;
}
