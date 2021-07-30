#include <stdio.h>

void r(); void g(int c); void a(int v); 
void f(int v); void h(int v); void b(); void x();

void r() {

    g(0); g(1);
    a(0); a(0); a(0); a(1); 
}

void g(int c) {

    
    if (c == 1) { 
        f(0); f(0); f(0); 
        h(2); h(2); h(2); h(2); h(2); h(0); 
    }

    
}

void f(int v) {

    if (v == 1) {
    
        b(); b(); b(); 
        b(); b(); b(); b(); 
    }
}

void a(int v) {

    if (v == 1) {
    
        h(2); h(2); h(1);
    }

}

void h(int v) {

    if (v == 0) {
    
        f(0); f(0); f(0); f(0); f(1);
    } 

    if (v == 1) {
    
        x(); x(); x(); x(); 
        x(); x(); x(); x(); 
    }
}

void x() { }
void b() { }

int main(int argc, char ** argv) {

    r();


    return 0;
}