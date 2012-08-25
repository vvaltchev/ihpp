#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *foo(void *arg) {

	int i=0;

	while(1) {

		if (arg) {
		
			if (i >= 3)
				break;	
		} else {

			if (i >= 9)
				break;
		}
		
		printf("%i\n",i);
		i++;
	}

	return 0;
}

int main(int argc, char ** argv) {

	pthread_t th;
	
	foo((void*)1);

	pthread_create(&th, NULL, foo, NULL);
	pthread_join(th, NULL);

	return 0;
}
