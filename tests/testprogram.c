#include <stdio.h>
#include <setjmp.h>


void foo() {

	printf("foo(), line 5\n");
}

int factorial(int n) {

	int r;

	printf("factorial, n=%i\n", n);

	if (n == 1)
		return 1;

	r = n*factorial(n-1);

	printf("factorial r*5(n=%i): %i\n", n, r*5);

	if (r*5 > 100) {

		printf("5r > 100\n");
	} else {
	
		printf("5r < 100\n");
	}


	return r;
}

int varfunc(int num, ...) {

	if (!num)
		goto end;

	varfunc(num-1, 0, 0);
	
	end:
	return 23;
}

jmp_buf jump_buffer;

void testJmp(int par, ...) {

	//printf("begin testJmp\n");

	if (par) 
		longjmp(jump_buffer, 1);

	//printf("end testjmp\n");
}

void c() {
	printf("c\n");
}

void b() {
	printf("b\n");
	c();
}

void a() {
	printf("a\n");
	b();
}

int main(int argc, char ** argv) {

int i,x=0;
   
//getchar();

printf("size of int: %lu\n", sizeof(int));
printf("size of long: %lu\n", sizeof(long));
printf("size of long long: %lu\n", sizeof(long long));
   
printf("ptr: %u bits\n", 8*sizeof(void*));
printf("factorial(6): %i\n", factorial(6));
//printf("factorial(3): %i\n", factorial(3));

varfunc(2, 20, 30);
varfunc(4, 99, 567, 11, 444);
varfunc(2, 80, 90);

a();



for (i=1; i <= 3; i++) {
	printf("x=%i\n",x);
	x+=i;
}
//exceptionFunc();
//exceptionFunc();
//exceptionFunc();


if (!setjmp(jump_buffer)) {

	//printf("main, before testJmp(1)\n");
	testJmp(1);
	//printf("main, AFTER testJmp(1)\n");

} else {

	//printf("main, LONG JUMP avvenuto\n");

	//printf("main, before testJmp(0)\n");
	testJmp(0, 434, 24324, 23411);
	//printf("main, AFTER testJmp(0)\n");

}



printf("4\n");

//for (int i=0; i < 10; i++)
//	foo();

return 0;
}
