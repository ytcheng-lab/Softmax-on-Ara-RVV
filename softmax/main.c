#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "kernel/softmax.h"
#include "runtime.h"
#include "util.h"

#ifndef SPIKE
#include "printf.h"
#else
#include <stdio.h>
#endif

#define RVV
#define CHECK

// Check the results using a threshold
#define THRESHOLD 0.0001

extern int N;
extern float i[] __attribute__((aligned(8)));
extern float o_g[] __attribute__((aligned(8)));
extern float o_t[] __attribute__((aligned(8)));
extern float buf[] __attribute__((aligned(8)));

void print_arr(float*, int);

int main() {
    printf("=======================================================\n");
    printf("Softmax Benchmark: %d float32 inputs [0, 10)\n", N);
    printf("Target: Compare RV64GC with/without RVV, Measured via mcycle (0xc00)\n");
    printf("=======================================================\n");
    #ifdef FILEIO
        // debug with file output
        FILE *fp;
        fp = fopen("output.txt","w");
    #endif

    int64_t cycles_baseline, cycles_speedup;
    int error = 0;

    start_timer();
    softmax(i, o_g, buf, N);
    stop_timer();
    cycles_baseline = get_timer();
    #ifdef DEBUG
        print_arr(o_g, N);
    #endif

    #ifdef RVV
        start_timer();
        softmax_vec3(i, o_t, buf, N);
        stop_timer();
        cycles_speedup = get_timer();
    #else
        start_timer();
        softmax_expth(i, o_t, buf, N);
        stop_timer();
        cycles_speedup = get_timer();
    #endif

    #ifdef DEBUG
        print_arr(o_t, N);
    #endif

    #ifdef FILEIO
        for (int i = 0; i < N; i++) {
            fprintf(fp, "%f\n", o_g[i]);
        }
    #endif

    #ifdef CHECK
        float abs_err;
        float max_err = .0f;
        for (int k = 0; k < N; ++k) {
            abs_err = o_t[k] - o_g[k];
            abs_err *= (abs_err > 0)? 1 : -1;
            max_err = (abs_err > max_err)? abs_err : max_err;
        }
    #endif

    float result = cycles_speedup / 1.0 / cycles_baseline;
    float speedup_percentage = (1 - result) * 100.0f;

    printf("=======================================================\n");
    printf("                         Result                        \n");
    printf("=======================================================\n");
    printf("[Baseline - no RVV]       %u cycles\n", cycles_baseline);
    printf("[Optimized-    RVV]       %u cycles\n", cycles_speedup);
    printf("[Speedup]   %.2f%% time reduction\n", speedup_percentage);
    printf("[Accuracy]  Max absolute error: %.4f\n", max_err);

    return error;
}

#ifdef DEBUG
void print_arr(float* arr, int N) {
    for(int i = 0; i < N; i++){
        printf("%f ", arr[i]);
    }
    printf("\n");
}
#endif
