#include <stdio.h>
#include <stdlib.h>

#define ERROR_STATUS -1
#define BITS_PER_BYTE 8
#define NO_BUCKETS 2 << 11

typedef struct info {
    char *key;
    char value;
} info;

typedef struct Hashtable {
    info *buckets;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void*, void*);
} Hashtable;

void file_check(FILE *f) {
    if (f == NULL) {
        fprintf(stderr, "Couldn't open input file\n");
        exit(ERROR_STATUS);
    }
}

void init_ht(Hashtable *ht, int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*)) {
    if (ht == NULL) {
        return;
    }

    ht->hmax = hmax;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;

    // Alocarea si initialziarea listelor inlantuite
    ht->buckets = calloc(hmax, sizeof(info));
}

int compare_function_ints(void *a, void *b) {
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a == int_b) {
        return 0;
    } else if (int_a < int_b) {
        return -1;
    } else {
        return 1;
    }
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

int log_2(int x) {
    /*
    * Credits: https://stackoverflow.com/questions/14767308/how-to-computer-log-base-2-using-bitwise-operators
    */
    int answer = 0;
    while(x >>= 1) {
        answer++;
    }
}

int max(int a, int b) {
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
    int bucket_no, zeros_cnt, x, no, hash;
    int m = NO_BUCKETS;
    int log_m = log_2(m);
    int *M = calloc(m + 1, sizeof(int));
    mem_check(M);

    Hashtable *ht = malloc(sizeof(Hashtable));
    mem_check(ht);
    init_ht(ht, m, hash_function_int, compare_function_ints);

    // 2) Processing input
    while(fscanf(in, "%d", &x) != EOF) {
        hash = ht->hash_function(&x);
 
        bucket_no = hash >> (no_bits_total - log_m);
        zeros_cnt = find_initial_zeros(x, no_bits_total, log_m);
        
        M[bucket_no] = max(M[bucket_no], zeros_cnt);
    }

    // 3) Aggregating values
    double z = 0;

    for (int i = 0; i < m; i++) {
        z += (double)1 / (2 << M[i]);
    }

    z = 1/z;

    // 4) Determining final answer
    double E;
    double alpha = (double)0.7213 / (1 + (double)1.079 / m);
    E = alpha * (m * m) * z;
    E *= 2;

    printf("%f\n", E);

    fclose(in);
    free(M);
    free_ht(ht);
    return 0;
}