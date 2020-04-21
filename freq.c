// Copyright 2020 Radu-Stefan Minea 314CA

#include <stdio.h>
#include <stdlib.h>

#define ERROR_STATUS -1
#define NMAX 2000001

void mem_check(void *p);

int main() {
  unsigned char *freq_array = calloc(NMAX, sizeof(unsigned char));
  mem_check(freq_array);

  int x;

  // Adding input to freq_array
  while (fscanf(stdin, "%d", &x) != EOF) {
    freq_array[x]++;
  }

  // If "i" was among the input, show its frequency
  for (int i = 0; i < NMAX; i++) {
    if (freq_array[i]) {
      printf("%d %d\n", i, freq_array[i]);
    }
  }

  free(freq_array);
  return 0;
}

void mem_check(void *p) {
  if (p == NULL) {
    fprintf(stderr, "Error allocating memory\n");
    exit(ERROR_STATUS);
  }
}
