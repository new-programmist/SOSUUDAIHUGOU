#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <time.h>
#include <stdbool.h>

#define MAX_PRIME_FACTOR 10000
#define MIN_DIGITS 19

typedef struct {
    mpz_t prime;
    unsigned long exponent;
} Factor;

typedef struct {
    Factor *factors;
    size_t count;
} Factorization;

void init_factorization(Factorization *f) {
    f->factors = NULL;
    f->count = 0;
}

void add_factor(Factorization *f, const mpz_t prime, unsigned long exponent) {
    f->factors = realloc(f->factors, (f->count + 1) * sizeof(Factor));
    mpz_init(f->factors[f->count].prime);
    mpz_set(f->factors[f->count].prime, prime);
    f->factors[f->count].exponent = exponent;
    f->count++;
}

void clear_factorization(Factorization *f) {
    for (size_t i = 0; i < f->count; i++) {
        mpz_clear(f->factors[i].prime);
    }
    free(f->factors);
}

bool factorize(const mpz_t n, Factorization *factors) {
    mpz_t remainder, factor, temp;
    mpz_inits(remainder, factor, temp, NULL);
    mpz_set(remainder, n);
    
    if (mpz_cmp_ui(remainder, 1) == 0) {
        mpz_clears(remainder, factor, temp, NULL);
        return true;
    }
    
    // Check for small factors first
    unsigned long small_factors[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31};
    for (size_t i = 0; i < sizeof(small_factors)/sizeof(small_factors[0]); i++) {
        unsigned long p = small_factors[i];
        if (mpz_cmp_ui(remainder, p * p) < 0) break;
        
        unsigned long exponent = 0;
        while (mpz_divisible_ui_p(remainder, p)) {
            exponent++;
            mpz_divexact_ui(remainder, remainder, p);
        }
        
        if (exponent > 0) {
            mpz_set_ui(factor, p);
            add_factor(factors, factor, exponent);
        }
    }
    
    // Pollard's Rho algorithm for larger factors
    while (mpz_cmp_ui(remainder, 1) > 0) {
        if (mpz_probab_prime_p(remainder, 15) > 0) {
            add_factor(factors, remainder, 1);
            break;
        }
        
        // Simple factorization attempt (for demo purposes)
        // In production code, you'd want a proper factorization algorithm
        mpz_set_ui(factor, 2);
        bool found = false;
        while (mpz_cmp(factor, remainder) < 0) {
            if (mpz_divisible_p(remainder, factor)) {
                unsigned long exponent = 0;
                mpz_set(temp, remainder);
                while (mpz_divisible_p(temp, factor)) {
                    exponent++;
                    mpz_divexact(temp, temp, factor);
                }
                add_factor(factors, factor, exponent);
                mpz_set(remainder, temp);
                found = true;
                break;
            }
            mpz_add_ui(factor, factor, 1);
            if (mpz_cmp_ui(factor, MAX_PRIME_FACTOR) > 0) {
                mpz_clears(remainder, factor, temp, NULL);
                return false;
            }
        }
        
        if (!found) {
            mpz_clears(remainder, factor, temp, NULL);
            return false;
        }
    }
    
    mpz_clears(remainder, factor, temp, NULL);
    return true;
}

void generate_number(mpz_t result) {
    const char *digits[] = {"13", "12", "11", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    char buffer[200] = {0};
    
    for (int i = 0; i < 12; i++) {
        int repeats = rand() % 3;
        for (int j = 0; j < repeats; j++) {
            strcat(buffer, digits[i]);
        }
    }
    
    mpz_set_str(result, buffer, 10);
}

void print_factorization(const mpz_t n, const Factorization *factors) {
    mpz_out_str(stdout, 10, n);
    printf(" = ");
    
    for (size_t i = 0; i < factors->count; i++) {
        if (i > 0) printf("*");
        mpz_out_str(stdout, 10, factors->factors[i].prime);
        if (factors->factors[i].exponent > 1) {
            printf("^%lu", factors->factors[i].exponent);
        }
    }
    printf("\n");
}

int main() {
    srand(time(NULL));
    mpz_t a;
    mpz_init(a);
    
    while (1) {
        generate_number(a);
        
        // Check minimum digit count
        if (mpz_sizeinbase(a, 10) < MIN_DIGITS) {
            continue;
        }
        
        Factorization factors;
        init_factorization(&factors);
        
        if (factorize(a, &factors)) {
            // Check if all factors are below threshold
            bool valid = true;
            for (size_t i = 0; i < factors.count; i++) {
                if (mpz_cmp_ui(factors.factors[i].prime, MAX_PRIME_FACTOR) > 0) {
                    valid = false;
                    break;
                }
            }
            
            if (valid) {
                print_factorization(a, &factors);
            }
        }
        
        clear_factorization(&factors);
    }
    
    mpz_clear(a);
    return 0;
}
