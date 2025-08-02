#include "softmax.h"
#include "printf.h"
#include <math.h>
#include <riscv_vector.h>

#define C -0.693147180 //ln2 for fast log2
#define T5 8.3334519073E-3
#define T4 4.1665795894E-2
#define T3 1.6666665459E-1
#define T2 5.0000001201E-1
#define T1 1.0
#define FP32_SIZE 4 //for 4 bytes
#define MAX_AVL 128

//#define DEBUG

// RVV Taylor expansion N=3
void softmax_vec3(float* in, float* out, float* buf, int N) {
    size_t avl = MAX_AVL; // for VLEN=4096, avl = 128 at most
    size_t vl;
    vfloat32m1_t num_v;
    vfloat32m1_t buf_v;
    vfloat32m1_t res_v;
    vfloat32m1_t sum_v;
    float sum_s = .0f;
    float tmp;

    // pointers for input value traverse
    float* in_ptr = in;
    float* out_ptr = out;
    float* buf_ptr = buf;
    int tra_size = .0f;

    // set Taylor series settings, make then constant vector
    vfloat32m1_t zero_v = vfmv_v_f_f32m1(.0f, vl);
    vfloat32m1_t one_v = vfmv_v_f_f32m1(1.0f, vl); // T0 and T1 = vec of 1.0 
    vfloat32m1_t T2_v = vfmv_v_f_f32m1(T2, vl); 
    vfloat32m1_t T3_v = vfmv_v_f_f32m1(T3, vl); 
    vfloat32m1_t T4_v = vfmv_v_f_f32m1(T4, vl); 
    vfloat32m1_t T5_v = vfmv_v_f_f32m1(T5, vl);

    // Step 1, exp(x) save to buffer array
    //         , sum saved to sum_s
 
    // set vector settings
    vl = vsetvl_e32m1(avl); //SEW=32, LMUL=1

    while(tra_size < N)
    {
        // from input array load 128 to vector register
        num_v = vle32_v_f32m1(in_ptr, vl); // input value
        buf_v = vle32_v_f32m1(in_ptr, vl); // sum for poly result

        // accumulate and multiplication
        // sum = x*T3 + T2
        buf_v = vfmadd_vv_f32m1(buf_v, T3_v, T2_v, vl);
        // sum = sum*x + T1 = sum*x + 1
        buf_v = vfmadd_vv_f32m1(buf_v, num_v, one_v, vl);
        // sum = x*sum + 1 = sum*x + 1
        buf_v = vfmadd_vv_f32m1(buf_v, num_v, one_v, vl);

        // store buffer array
        vsse32_v_f32m1(buf_ptr, FP32_SIZE, buf_v, vl);

        // how to add up all elements and return scalar?
        // vfloat32m1_t vfredsum_vs_f32m1_f32m1(dest, vector, scalar, size)
        sum_v = vfredusum_vs_f32m1_f32m1(sum_v, buf_v, zero_v, vl);
        vsse32_v_f32m1(&tmp, FP32_SIZE, sum_v,1);
        sum_s += tmp;

	#ifdef DEBUG
            printf("softmax_vec::sum: %f\n", sum_s);
	#endif

        // traversed vl numbers, move to next
        buf_ptr += vl;
        in_ptr += vl;
        tra_size += vl;
     }

    // Step 2, divide with sum of exp(i)
    tra_size = 0; //reset
    buf_ptr = buf; //reset
    while (tra_size < N)
    {

        // load from buffer
        num_v = vle32_v_f32m1(buf_ptr, vl);

        // devide each element
	res_v = vfdiv_vf_f32m1(num_v, sum_s, vl);	
	// store to output
        vsse32_v_f32m1(out_ptr, FP32_SIZE, res_v, vl);

        buf_ptr += vl;
        out_ptr += vl;
        tra_size += vl;

    }

}

// RVV Taylor expansion N=5
void softmax_vec5(float* in, float* out, float* buf, int N) {
    size_t avl = MAX_AVL; // for VLEN=4096, avl = 128 at most
    size_t vl;
    vfloat32m1_t num_v;
    vfloat32m1_t buf_v;
    vfloat32m1_t res_v;
    vfloat32m1_t sum_v;
    float sum_s = .0f;
    float tmp;

    // pointers for input value traverse
    float* in_ptr = in;
    float* out_ptr = out;
    float* buf_ptr = buf;
    int tra_size = .0f;

    // set Taylor series settings, make then constant vector
    vfloat32m1_t zero_v = vfmv_v_f_f32m1(.0f, vl);
    vfloat32m1_t one_v = vfmv_v_f_f32m1(1.0f, vl); // T0 and T1 = vec of 1.0 
    vfloat32m1_t T2_v = vfmv_v_f_f32m1(T2, vl); 
    vfloat32m1_t T3_v = vfmv_v_f_f32m1(T3, vl); 
    vfloat32m1_t T4_v = vfmv_v_f_f32m1(T4, vl); 
    vfloat32m1_t T5_v = vfmv_v_f_f32m1(T5, vl);

    // Step 1, exp(x) save to buffer array
    //         , sum saved to sum_s
 
    // set vector settings
    vl = vsetvl_e32m1(avl); //SEW=32, LMUL=1

    while(tra_size < N)
    {
        // from input array load 128 to vector register
        num_v = vle32_v_f32m1(in_ptr, vl); // input value
        buf_v = vle32_v_f32m1(in_ptr, vl); // sum for poly result

        // accumulate and multiplication
        // sum = x*T5 + T4
        // vfmadd_vv_f32m4()
        // vfmadd.vv vd, vs1, vs2, vm
        // vd = (vd*vs1) + vs2
        buf_v = vfmadd_vv_f32m1(buf_v, T5_v, T4_v, vl);
        // sum = sum*x + T3
        buf_v = vfmadd_vv_f32m1(buf_v, num_v, T3_v, vl);
        // sum = sum*x + T2
        buf_v = vfmadd_vv_f32m1(buf_v, num_v, T2_v, vl);
        // sum = sum*x + T1 = sum*x + 1
        buf_v = vfmadd_vv_f32m1(buf_v, num_v, one_v, vl);
        // sum = x*sum + 1 = sum*x + 1
        buf_v = vfmadd_vv_f32m1(buf_v, num_v, one_v, vl);

        // store buffer array
        vsse32_v_f32m1(buf_ptr, FP32_SIZE, buf_v, vl);

        // how to add up all elements and return scalar?
        // vfloat32m1_t vfredsum_vs_f32m1_f32m1(dest, vector, scalar, size)
        sum_v = vfredusum_vs_f32m1_f32m1(sum_v, buf_v, zero_v, vl);
        vsse32_v_f32m1(&tmp, FP32_SIZE, sum_v,1);
        sum_s += tmp;

	#ifdef DEBUG
            printf("softmax_vec::sum: %f\n", sum_s);
	#endif

        // traversed vl numbers, move to next
        buf_ptr += vl;
        in_ptr += vl;
        tra_size += vl;
     }

    // Step 2, divide with sum of exp(i)
    tra_size = 0; //reset
    buf_ptr = buf; //reset
    while (tra_size < N)
    {

        // load from buffer
        num_v = vle32_v_f32m1(buf_ptr, vl);

        // devide each element
	res_v = vfdiv_vf_f32m1(num_v, sum_s, vl);	
	// store to output
        vsse32_v_f32m1(out_ptr, FP32_SIZE, res_v, vl);

        buf_ptr += vl;
        out_ptr += vl;
        tra_size += vl;

    }

}


void softmax(float* in, float* out, float* buf, int N) {
    float exp_sum = .0f;
    for (int i = 0; i < N; i++) {
	buf[i] = exp(in[i]);
	exp_sum += buf[i];
    }
    #ifdef DEBUG
        printf("golden::exp_sum = %lf\n",exp_sum);
    #endif

    for (int i = 0; i < N; i++) {
        out[i] = buf[i] / exp_sum;
    }

}

void softmax_expth(float* in, float* out, float* buf, int N) {
    float exp_sum = .0f;
    float x_;
    for (int i = 0; i < N; i++) {
    	x_ = in[i]+10; // shift to [0-40), to compare to LUT usage
	buf[i] = expth(x_);
        buf[i] = expth(in[i]);
	exp_sum += buf[i];
    }
    #ifdef DEBUG
        printf("exp_sum = %lf\n",exp_sum);
    #endif
    for (int i = 0; i < N; i++) {
        out[i] = buf[i] / exp_sum;
    }
}


inline float expth (float x) {
	// Taylor expansion for N = 3
	float sum = x*T3 + T2;
	sum = x*sum + T1;
	sum = x*sum + 1;
	return sum;
}
