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
    long unsigned int digit, i, div, mod, th;
    long unsigned int digits[d + 11];
    int num_threads = omp_get_max_threads();
    long unsigned int aux[num_threads][d + 11];
    int j;
    

    #pragma omp parallel private(digit, div, mod, i)
    {
        int ithread = omp_get_thread_num();
        int nthread = omp_get_num_threads();
        long unsigned int remainder = 1;

        long unsigned int _start = ((ithread * (d + 11)) / nthread);
        long unsigned int _end = (((ithread + 1) * (d+11)) / nthread);  

        for(digit = _start; digit < _end; ++digit){
            digits[digit] = 0;
            for(j = 0; j < nthread; j++) aux[j][digit] = 0;
        }

        long unsigned int start = ((ithread * n) / nthread) + 1;
        long unsigned int end = (((ithread + 1) * n) / nthread) + 1; 
 
        for(i = start; i <= end; ++i){
            remainder = 1;
            for (digit = 0; digit < d + 11 && remainder; ++digit) {
                div = remainder / i;
                mod = remainder % i;
                aux[ithread][digit] += div;
                remainder = mod * 10;
            }
        }
    }

    #pragma omp parallel for private(th)
    for(digit = 0; digit < d + 11; ++digit){
        for(th = 0; th <num_threads; ++th){
            digits[digit] += aux[th][digit];
        }
    }


    for (i = d + 11 - 1; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    if (digits[d + 1] >= 5) {
        ++digits[d];
    }

    for (i = d; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    sprintf(output,"%lu.",digits[0]);
    unsigned long int t = strlen(output);
    
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