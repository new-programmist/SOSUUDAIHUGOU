#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int scaledrand(int x) {
    if (x == 0) return 0;
    int a = rand() % (x + 1);
    int b = rand() % x;
    return a > b ? a : b;
}

void generate(const char* text, int** elements_ptr, int* elements_size) {
    int counts[14] = {0};
    for (const char* p = text; *p != '\0'; p++) {
        char c = *p;
        int value;
        switch (c) {
            case 'A': value = 1; break;
            case '2': value = 2; break;
            case '3': value = 3; break;
            case '4': value = 4; break;
            case '5': value = 5; break;
            case '6': value = 6; break;
            case '7': value = 7; break;
            case '8': value = 8; break;
            case '9': value = 9; break;
            case 'T': value = 10; break;
            case 'J': value = 11; break;
            case 'Q': value = 12; break;
            case 'K': value = 13; break;
            case 'O': value = rand() % 14; break;
            //case 'O': value = 1 + rand() % 13; break;
            default: value = -1; // Skip invalid characters
        }
        if (value >= 0 && value <= 13) {
            counts[value]++;
        }
    }

    int* elements = NULL;
    size_t capacity = 0;
    size_t size = 0;
    for (int i = 0; i < 14; i++) {
        if (counts[i] == 0) continue;
        int s = scaledrand(counts[i]);
        for (int j = 0; j < s; j++) {
            if (size >= capacity) {
                capacity = (capacity == 0) ? 1 : capacity * 2;
                elements = realloc(elements, capacity * sizeof(int));
            }
            elements[size++] = i;
        }
    }

    // Fisher-Yates shuffle only if there are elements to shuffle
    if (size > 0) {
        for (size_t i = 0; i < size - 1; i++) {
            size_t j = i + rand() % (size - i);
            int temp = elements[j];
            elements[j] = elements[i];
            elements[i] = temp;
        }
    }

    *elements_ptr = elements;
    *elements_size = size;
}

// The rest of the functions remain unchanged

char* concatenate_numbers(int* array, int size) {
    int total_length = 0;
    for (int i = 0; i < size; i++) {
        total_length += snprintf(NULL, 0, "%d", array[i]);
    }
    char* result = malloc(total_length + 1);
    result[0] = '\0';
    for (int i = 0; i < size; i++) {
        char num_str[20];
        snprintf(num_str, sizeof(num_str), "%d", array[i]);
        strcat(result, num_str);
    }
    return result;
}

unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long mod) {
    unsigned long long res = 0;
    a %= mod;
    while (b > 0) {
        if (b % 2 == 1) {
            res = (res + a) % mod;
        }
        a = (a * 2) % mod;
        b /= 2;
    }
    return res % mod;
}

// Function to compute (base^exp) % mod efficiently
unsigned long long powmod(unsigned long long base, unsigned long long exp, unsigned long long mod) {
    unsigned long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            res = mulmod(res, base, mod);
        }
        base = mulmod(base, base, mod);
        exp /= 2;
    }
    return res;
}

// Miller-Rabin primality test
bool is_prime(unsigned long long n, int k) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0) return false;
    
    // Write n-1 as d*2^s
    unsigned long long d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }
    
    // Test for k witnesses
    for (int i = 0; i < k; i++) {
        // Choose a random number between [2, n-2]
        unsigned long long a = 2 + rand() % (n - 3);
        unsigned long long x = powmod(a, d, n);
        
        if (x == 1 || x == n - 1) continue;
        
        int j;
        for (j = 0; j < s - 1; j++) {
            x = mulmod(x, x, n);
            if (x == n - 1) break;
        }
        
        if (j == s - 1) return false; // composite
    }
    
    return true; // probably prime
}

void print_comma_separated(int* array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d", array[i]);
        if (i < size - 1) {
            printf(",");
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "素数v2:\n");
        fprintf(stderr, "Usage: %s n [カード]\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    char* text = (argc >= 3) ? argv[2] : "A23456789TJQK";
    srand(time(NULL));

    unsigned long long* arr = NULL;
    size_t arr_size = 0;

    for (int i = 0; i < n; i++) {
        int* elements;
        int elements_size;
        generate(text, &elements, &elements_size);

        char* concat = concatenate_numbers(elements, elements_size);
        unsigned long long num = strtoull(concat, NULL, 10);
        free(concat);

        if (is_prime(num,40)) {
            bool should_add = arr_size == 0 || num > arr[arr_size - 1];
            printf("%s\n", should_add ? "" : "");
            should_add = true;
            if (should_add) {
                print_comma_separated(elements, elements_size);
                printf("   ");
                arr = realloc(arr, (arr_size + 1) * sizeof(unsigned long long));
                arr[arr_size++] = num;
            }
        }
        free(elements);
    }

    printf("\n");
    free(arr);
    return 0;
}
