void bar(int n); void f3() { }

void foo() { 

    bar(0); 
    f3(); 
    bar(1); 
}

void bar(int n) { 

    if (n) 
        f3(); 
}
int main(int argc, char ** argv) { foo(); return 0; }
