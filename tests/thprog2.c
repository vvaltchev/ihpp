

void foo(); void bar();

void recFunc(int n) {

    if (!n) { bar(); return; }
    recFunc(n-1);
}

void foo() { recFunc(5); }
void bar() { }

int main(int argc, char ** argv) { foo(); return 0; }
