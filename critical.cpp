/* sum.c (Roland Teodorowitsch; 16 set. 2021)
 * Compilation: gcc sum.c -o sum -fopenmp
 * Adapted from: http://lspd.mackenzie.br/marathon/06/sum.zip
 */

#include <stdio.h>
#include <string.h>
#include <omp.h>

#define START  1000000
#define STEP   1000000
#define END   10000000

#define DIGITS 1000

void sum(char* output, const long unsigned int d, const long unsigned int n) {
    long unsigned int digit, i, remainder, div, mod;
    long unsigned int digits[d + 11];

    // #pragma omp parallel for
    for (digit = 0; digit < d + 11; ++digit) {
        digits[digit] = 0;
    }
    // long unsigned int digits[d + 11][num_thread];
    #pragma omp parallel for private(digit, div, mod, remainder)
    for (i = 1; i <= n; ++i) {
        remainder = 1;
        for (digit = 0; digit < d + 11 && remainder; ++digit) {
            div = remainder / i;
            mod = remainder % i;
            #pragma omp critical
            digits[digit] += div;
            remainder = mod * 10;
        }
    }

    // Não dá para criar paralelismo, pois as execuções possuem dependências diretas entre si
    for (i = d + 11 - 1; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    if (digits[d + 1] >= 5) {
        ++digits[d];
    }

    // Não dá para criar paralelismo, pois as execuções possuem dependências diretas entre si
    for (i = d; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    sprintf(output,"%lu.",digits[0]);
    unsigned long int t = strlen(output);
    
    // Não vale a pena criar paralelismo dado o custo para criar as threads e que deverá ser incrementado a variável T para cada execução
    for (i = 1; i <= d; ++i)
        output[t++] = digits[i]+'0';

    output[t] = '\0';
}

int main() {
    long unsigned int n;
    double start, finish;
    char output[DIGITS + 10]; // extra chars to avoid error
    for (n=START; n<=END; n+=STEP) {
        start = omp_get_wtime();
        sum(output, DIGITS, n);
        finish = omp_get_wtime();  
        fprintf(stdout,"%s\n",output);
        fprintf(stderr,"%lu %lf\n",n,finish-start);
    }
    // testando 
    return 0;
}