// Copyright 2020 Radu-Stefan Minea 314CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 101
#define ERROR_STATUS -1
#define NEW_STRING_CNT 1

typedef struct info {
  char *key;
  char value;
} info;

typedef struct Hashtable {
  info *buckets;
  int hmax;
  unsigned int (*hash_function)(void *);
  int (*compare_function)(void *, void *);
} Hashtable;

void init_ht(Hashtable *ht, int hmax, unsigned int (*hash_function)(void *),
             int (*compare_function)(void *, void *));
int compare_function_strings(void *a, void *b);
unsigned int hash_function_string(void *a);
void put(struct Hashtable *ht, void *key, int key_size_bytes);
void free_ht(Hashtable *ht);
void mem_check(void *p);

int main() {
  int cnt = 0;
  int key_size_bytes;
  char buffer[MAX_LEN];
  char **temp = malloc(cnt * sizeof(char *));

  // Manual mem_check, becuase the function was defined for pointers,
  // not for double pointers
  if (temp == NULL) {
    fprintf(stderr, "Error allocating memory\n");
    exit(ERROR_STATUS);
  }

  // Finding max no. of buckets
  while (fscanf(stdin, "%s", buffer) != EOF) {
    temp = realloc(temp, (cnt + 1) * sizeof(char *));

    if (temp == NULL) {
      fprintf(stderr, "Error allocating memory\n");
      exit(ERROR_STATUS);
    }

    // Allocating one extra byte, otherwise snprintf would exclude the last
    // character of the string
    temp[cnt] = malloc(sizeof(buffer) + 1);
    snprintf(temp[cnt], sizeof(buffer) + 1, "%s", buffer);
    cnt++;
  }

  // Initializing hashtable
  Hashtable *ht = malloc(sizeof(Hashtable));
  mem_check(ht);
  init_ht(ht, cnt, hash_function_string, compare_function_strings);

  // Transferring input to hashtable
  for (int i = 0; i < cnt; i++) {
    key_size_bytes = strlen(temp[i]);
    put(ht, temp[i], key_size_bytes);
    free(temp[i]);
  }

  // Printing results
  for (int i = 0; i < cnt; i++) {
    if (ht->buckets[i].value) {
      info inside_data = ht->buckets[i];
      printf("%s %d\n", inside_data.key, inside_data.value);
    }
  }

  // Freeing
  free(temp);
  free_ht(ht);

  return 0;
}

void init_ht(Hashtable *ht, int hmax, unsigned int (*hash_function)(void *),
             int (*compare_function)(void *, void *)) {
  if (ht == NULL) {
    return;
  }

  ht->hmax = hmax;
  ht->hash_function = hash_function;
  ht->compare_function = compare_function;

  ht->buckets = calloc(hmax, sizeof(info));
  mem_check(ht->buckets);
}

int compare_function_strings(void *a, void *b) {
  char *str_a = (char *)a;
  char *str_b = (char *)b;

  return strcmp(str_a, str_b);
}

unsigned int hash_function_string(void *a) {
  /*
   * Credits: http://www.cse.yorku.ca/~oz/hash.html
   */
  unsigned char *puchar_a = (unsigned char *)a;
  unsigned int hash = 5381;
  int c;

  while ((c = *puchar_a++))
    hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

  return hash;
}

void put(struct Hashtable *ht, void *key, int key_size_bytes) {
  if (ht == NULL) {
    return;
  }

  int hash = ht->hash_function(key) % ht->hmax;
  info *inside_data = &ht->buckets[hash];

  // Linear probing; if initial bucket was empty => while loop isn't accessed
  while (inside_data->value &&
         ht->compare_function(key, ht->buckets[hash].key)) {
    hash = (hash + 1) % ht->hmax;
    inside_data = &ht->buckets[hash];
  }

  // OLD STRING => updating count
  if (inside_data->value) {
    inside_data->value++;
    return;
  }

  // NEW STRING => initializing count
  inside_data->key = malloc(key_size_bytes + 1);
  snprintf(inside_data->key, key_size_bytes + 1, "%s", (char *)key);
  inside_data->value = NEW_STRING_CNT;
}

void free_ht(Hashtable *ht) {
  if (ht == NULL) {
    return;
  }

  for (int i = 0; i < ht->hmax; i++) {
    free(ht->buckets[i].key);
  }

  free(ht->buckets);
  free(ht);
}

void mem_check(void *p) {
  if (p == NULL) {
    fprintf(stderr, "Error allocating memory\n");
    exit(ERROR_STATUS);
  }
}
