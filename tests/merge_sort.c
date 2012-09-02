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

void merge(int v[], int temp[], int a, int m, int b) {
    int k, i = a, j = m;
    for (k = a; k < b; k++)
        if (i < m && j < b)
            if (v[i] < v[j]) temp[k] = v[i++];
            else temp[k] = v[j++];
        else if (i < m) temp[k] = v[i++];
        else temp[k] = v[j++];
    memcpy(v+a, temp+a, (b-a)*sizeof(int));
}

void merge_sort_ric(int v[], int temp[], int a, int b) {
    int m;
    if (a + 1 >= b) return;
    m = (a + b)/2;
    merge_sort_ric(v, temp, a, m);
    merge_sort_ric(v, temp, m, b);
    merge(v, temp, a, m, b);
}

void merge_sort(int v[], int n) {
    int* temp = (int*)malloc(n*sizeof(int));
    if (temp == NULL) exit(1);
    merge_sort_ric(v, temp, 0, n);
	free(temp);
}

int main() {

	int i;
	double timer = getMilliseconds();

	for (i=0; i < 100*1000; i++) {
		int v[] = { 5, 2, 4, 8, 5, 8, 3, 2, 9, 0, 12, 4, 25, 5, 6};
		int n = sizeof(v)/sizeof(int), i;
		merge_sort(v, n);
	}

	timer = getMilliseconds() - timer;
	printf("[ test program ] time elapsed: %.3f sec\n", timer/1000.0);
    return 0;
}

