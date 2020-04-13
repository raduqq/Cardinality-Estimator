#include <stdio.h>
#include <stdlib.h>

#define NMAX 2000001 

int main() {
    unsigned char *freq_array = calloc(NMAX, sizeof(unsigned char));
    int x;

    while(fscanf(stdin, "%d", &x) != EOF) {
        freq_array[x]++;
    }

    for (int i = 0; i < NMAX; i++) {
        if (freq_array[i]) {
            printf("%d %d\n", i, freq_array[i]);
        }
    }

    free(freq_array);
    return 0;
}