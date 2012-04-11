
void a();
void b();


void a(int c) {

printf("a(%i)\n",c);


if (!c)
return;


b(c-1);

}

void b(int c) {

printf("b(%i)\n",c);

if (!c)
return;

a(c-1);
}

int main() {

a(5);

return 0;
}