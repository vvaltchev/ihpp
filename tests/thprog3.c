void bar(int n);

void foo(int n) {
    
    if (!n) return;
    bar(n-1);
}

void bar(int n) {

    if (!n) return;
    foo(n-1);
}

int main(int argc, char ** argv) { foo(5);    return 0; }
