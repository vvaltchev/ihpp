#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <windows.h>

double getMilliseconds() {

	return (double)GetTickCount();
}

#else

#include <time.h>
#include <sys/time.h>

double getMilliseconds() {

	struct timeval tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec*1000.0 + tv.tv_usec/1000.0;
}

#endif


int partition(int *v, int a, int b) {
	int temp;
	int pivot = a;
	for (;;) {
		while (a < b && v[a] <= v[pivot]) a++;
		while (v[b] > v[pivot]) b--;
		if (a >= b) break;
		temp = v[a];
		v[a] = v[b];
		v[b] = temp;
	}
	if (v[b] != v[pivot]) {
		temp = v[b];	
		v[b] = v[pivot];
		v[pivot] = temp;
	}
	return b;
}

void quick_sort_ric(int *v, int a, int b) {
    int m;
    if (a >= b) return;
    m = partition(v, a, b);
    quick_sort_ric(v, a, m-1);
    quick_sort_ric(v, m+1, b);
}

void quick_sort(int v[], int n) {
    quick_sort_ric(v, 0, n-1);
}

int main(void) {

	int i;
	double timer = getMilliseconds();

	for (i=0; i < 100*1000; i++) {
		
		int v[] = { 5, 2, 4, 8, 5, 8, 3, 2, 9, 0, 12, 4, 25, 5, 6};
		int n = sizeof(v)/sizeof(int);
 
		quick_sort(v, n);
	}

	timer = getMilliseconds() - timer;

	printf("[ test program ] time elapsed: %.3f sec\n", timer/1000.0);

    return 0;
}

