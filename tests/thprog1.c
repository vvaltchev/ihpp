#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void a(int x); void b(); void c();
void d(); void e(); void f();

void a(int x) { 

    if (!x) { 
        b(); c(); 
    } else { 
        b(); f();
    } 
}

void b() { }
void c() { }
void f() { }
void d() { c(); c(); }
void e() { d(); c(); a(0); }

void *thread2(void *arg) {

    a(1); return 0;
}

int main(int argc, char ** argv) {

    int r;
    pthread_t th;

    a(0); e(); e();

    r = pthread_create(&th, NULL, thread2, NULL);

    pthread_join(th, NULL);

    return 0;
}
