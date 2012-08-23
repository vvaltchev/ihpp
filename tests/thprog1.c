#include <stdio.h>

void a(); void b(); void c();
void d(); void e();

void a() { b(); c(); }

void b() { }
void c() { }
void d() { c(); c(); }
void e() { d(); c(); a(); }

int main(int argc, char ** argv) {

	a(); e(); e();

	return 0;
}