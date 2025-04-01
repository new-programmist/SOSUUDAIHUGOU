#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LEN 100

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

void shuffle(char *s, int len) {
    for (int i = 0; i < len; i++) {
        int j = rand()%len;
        char t = s[i]; s[i] = s[j]; s[j] = t;
    }
}

void proceed(char c, char *buf, int *idx) {
    c = toupper(c);
    if (c >= '1' && c <= '9') {
        buf[(*idx)++] = c;
    }
    else if (c == 'O') {
        int r = rand()%14;
        if (r < 10) {
            buf[(*idx)++] = '0' + r; // Store without increment
        } else {
            if (r == 10) strcpy(buf + *idx, "10");
            else if (r == 11) strcpy(buf + *idx, "11");
            else if (r == 12) strcpy(buf + *idx, "12");
            else if (r == 13) strcpy(buf + *idx, "13");
            *idx += 2;
        }
    }
    else if (c == 'A') buf[(*idx)++] = '1';
    else if (c == 'T') strcpy(buf + *idx, "10"), *idx += 2;
    else if (c == 'J') strcpy(buf + *idx, "11"), *idx += 2;
    else if (c == 'Q') strcpy(buf + *idx, "12"), *idx += 2;
    else if (c == 'K') strcpy(buf + *idx, "13"), *idx += 2;
    else buf[(*idx)++] = c;
}

void generate(char *text, int split, char *p1, char *p2) {
    int len = strlen(text);
    split = (split <= 0 || split >= len) ? rand()%(len-1)+1 : split;
    shuffle(text, len);
    char first[split+1], second[len-split+1];
    strncpy(first, text, split); first[split] = '\0';
    strncpy(second, text+split, len-split); second[len-split] = '\0';
    
    shuffle(first, split);
    shuffle(second, len-split);
    
    int i1 = 0, i2 = 0;
    for (int i = 0; i < split && i1 < MAX_LEN-2; i++) 
        proceed(first[i], p1, &i1);
    for (int i = 0; i < len-split && i2 < MAX_LEN-2; i++)
        proceed(second[i], p2, &i2);
    
    p1[i1] = p2[i2] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
      fprintf(stderr, "2発出し:\n");
      printf("Usage: %s times text [split]\n", argv[0]);
      return 1;
    }
    
    srand(time(0));
    int times = atoi(argv[1]), split = argc>3 ? atoi(argv[3]) : 0;
    
    while (times--) {
        char p1[MAX_LEN], p2[MAX_LEN];
        generate(argv[2], split, p1, p2);
        
        int n1 = atoi(p1), n2 = atoi(p2);
        if (is_prime(n1,40) && is_prime(n2,40)) { printf("[%d,%d]\n", n1, n2); } else { /*puts("NULL")*/};
    }
    return 0;
}
