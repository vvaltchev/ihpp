#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>

jmp_buf jump_buffer;

void testJmp(int par, ...) {

    va_list ap;

    if (par) 
        longjmp(jump_buffer, 1);

    printf("par is zero!\n");

    va_start(ap, par);

    while (par != -1) {

        par = va_arg(ap, int);
        printf("arg: %i\n", par);
    }
    va_end(ap);
}

int main(int argc, char ** argv) {

    if (!setjmp(jump_buffer)) {

        testJmp(1);
        printf("this message will not be printed!\n");
    
    } else {

        testJmp(0, 25, 100, 386, -1);
    }    

    return 0;
}
