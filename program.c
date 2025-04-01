#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <gmp.h>
#include <time.h>
#include <stdbool.h>

#define MAX_DIGITS 90
#define MIN_DIGITS 1
#define MAX_PRIMES 40000

typedef struct {
    mpz_t num;
} BigInt;

BigInt primes[MAX_PRIMES];
int prime_count = 0;
// Check if a number is prime and not already in the list
bool is_unique_prime(mpz_t num) {
    if (mpz_probab_prime_p(num, 25) == 0) {
        return false;
    }

    for (int i = 0; i < prime_count; i++) {
        if (mpz_cmp(primes[i].num, num) == 0) {
            return false;
        }
    }
    return true;
}

// Comparison function for sorting
int compare_primes(const void *a, const void *b) {
    const BigInt *pa = (const BigInt *)a;
    const BigInt *pb = (const BigInt *)b;
    return mpz_cmp(pa->num, pb->num);
}

void generate_number(mpz_t result, int argc, char *argv[]) {
    const char *digits[] = {"13", "12", "11", "10", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    char buffer[200] = {0}; // Large enough to hold 32-digit numbers

    for (int i = 0; i < 12; i++) {
        int max_repeats = (i < argc-1) ? atoi(argv[i+1]) : 0; // Use ARGV if provided, else default to 2
        int repeats = rand() % (max_repeats + 1);
        for (int j = 0; j < repeats; j++) {
            strcat(buffer, digits[i]);
        }
    }

    mpz_set_str(result, buffer, 10);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s 13 12 11 10 1 2 3 4 5 6 7 8 9\n", argv[0]);
    return 1;
  }
    srand(time(NULL));
    mpz_t a;
    mpz_init(a);

    // Initialize primes array
    for (int i = 0; i < MAX_PRIMES; i++) {
        mpz_init(primes[i].num);
    }

    for (int iteration = 0; iteration < 100000; iteration++) {
        generate_number(a, argc, argv);

        // Check number of digits
        size_t digits = mpz_sizeinbase(a, 10);
        if (digits > MAX_DIGITS || digits < MIN_DIGITS) {
            continue;
        }

        if (is_unique_prime(a)) {
            if (prime_count < MAX_PRIMES) {
                mpz_set(primes[prime_count].num, a);
                prime_count++;
                mpz_out_str(stdout, 10, a);
                printf("\n");
            }
        }
    }

    // Sort the primes
    qsort(primes, prime_count, sizeof(BigInt), compare_primes);

    // Print sorted primes
    printf("\nSorted primes:\n");
    for (int i = 0; i < prime_count; i++) {
        mpz_out_str(stdout, 10, primes[i].num);
        printf("\n");
    }

    // Clean up
    mpz_clear(a);
    for (int i = 0; i < MAX_PRIMES; i++) {
        mpz_clear(primes[i].num);
    }

    return 0;
}
