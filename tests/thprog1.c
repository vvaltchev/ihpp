#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void a(); void b(); void c();
void d(); void e();

void a() { b(); c(); }

void b() { }
void c() { }
void d() { c(); c(); }
void e() { d(); c(); a(); }

void *thread2(void *arg) {

	a(); return 0;
}

int main(int argc, char ** argv) {

	int r;
	pthread_t th;

	a(); e(); e();

	r = pthread_create(&th, NULL, thread2, NULL);

	pthread_join(th, NULL);

	return 0;
}
