// Copyright 2020 Radu-Stefan Minea 314CA

#include <stdio.h>
#include <stdlib.h>

#define ERROR_STATUS -1
#define BITS_PER_BYTE 8
#define NO_BUCKETS 2 << 11
#define BUCKET_BITS 11

typedef struct Hashtable {
  int hmax;
  unsigned int (*hash_function)(void *);
} Hashtable;

void file_check(FILE *f);
void init_ht(Hashtable *ht, int hmax, unsigned int (*hash_function)(void *));
void mem_check(void *p);
int my_max(int a, int b);
unsigned int hash_function_int(void *a);
int find_initial_zeros(int x, int no_bits_total, int no_bucket_bits);

int main(int argc, char **argv) {
  // 0) Checking command line paramaters
  if (argc < 2) {
    fprintf(stderr, "Please enter input file\n");
    exit(ERROR_STATUS);
  }

  if (argc > 2) {
    fprintf(stderr, "Please enter only one input file\n");
    exit(ERROR_STATUS);
  }

  FILE *in = fopen(argv[1], "r");
  file_check(in);

  // 1) Initializing variables
  int no_bits_total = sizeof(int) * BITS_PER_BYTE;
  int bucket_no, zeros_cnt, x, hash;
  int m = NO_BUCKETS;
  int *M = calloc(m, sizeof(int));
  mem_check(M);

  Hashtable *ht = malloc(sizeof(Hashtable));
  mem_check(ht);
  init_ht(ht, m, hash_function_int);

  // 2) Processing input
  while (fscanf(in, "%d", &x) != EOF) {
    hash = ht->hash_function(&x);
    if (hash < 0) {
      hash *= -1;
    }

    bucket_no = hash >> (no_bits_total - BUCKET_BITS);
    zeros_cnt = find_initial_zeros(hash, no_bits_total, BUCKET_BITS);
    M[bucket_no] = my_max(M[bucket_no], zeros_cnt);
  }

  // 3) Aggregating values
  double z = 0;
  int used_buckets = 0;
  for (int i = 0; i < m; i++) {
    if (M[i]) {
      z += (double)1 / (2 << M[i]);
      used_buckets++;
    }
  }
  z = 1 / z;

  // m becomes the number of used buckets
  m = used_buckets;

  // 4) Determining final answer
  int E;
  double alpha = (double)0.7213 / (1 + (double)1.079 / m);
  E = alpha * (m * m) * z;

  printf("%d\n", E);

  fclose(in);
  free(M);
  free(ht);
  return 0;
}

void file_check(FILE *f) {
  if (f == NULL) {
    fprintf(stderr, "Couldn't open input file\n");
    exit(ERROR_STATUS);
  }
}

void init_ht(Hashtable *ht, int hmax, unsigned int (*hash_function)(void *)) {
  if (ht == NULL) {
    return;
  }

  ht->hmax = hmax;
  ht->hash_function = hash_function;
}

void mem_check(void *p) {
  if (p == NULL) {
    fprintf(stderr, "Error allocating memory\n");
    exit(ERROR_STATUS);
  }
}

int my_max(int a, int b) {
  if (a > b) {
    return a;
  }
  return b;
}

unsigned int hash_function_int(void *a) {
  /*
   * Credits: https://stackoverflow.com/a/12996028/7883884
   */
  unsigned int uint_a = *((unsigned int *)a);

  uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
  uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
  uint_a = (uint_a >> 16u) ^ uint_a;
  return uint_a;
}

int find_initial_zeros(int x, int no_bits_total, int no_bucket_bits) {
  int cnt = 0;

  for (int i = no_bits_total - no_bucket_bits - 1; i >= 0; i--) {
    int aux = x >> i;
    if (aux % 2) {
      break;
    }
    cnt++;
  }

  return cnt;
}
