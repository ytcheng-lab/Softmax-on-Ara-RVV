# RVV Beginner Notes
This document summarizes the basics of writing and compiling C programs using RVV (RISC-V Vector Extension), focusing on its use in softmax acceleration experiments on the ARA platform.
Notice: include the RVV header in your C code:
```c
#include <riscv_vector.h>
``` 
All vector logic is implemented using RVV intrinsic functions.


**Compilation Environment**
- Requires LLVM version ≥ 16.0.0
  - Compile option to enable RVV ISA: -march=rv64gcv

## Key Terminology
|Term	|Meaning|
|-------|-------|
|VLEN	|Length of a vector register (in bits). For Ara: VLEN = 4096|
|ELEN	|Max element size (must be 2ⁿ, typically 8 ≤ ELEN ≤ VLEN)|
|SEW	|Standard Element Width (e.g., 32 for float)|
|LMUL	|Vector Length Multiplier, controls how many elements are grouped|
|AVL	|Application Vector Length (number of elements for current op)|

With VLEN=4096 and SEW=32, each vector register can hold 128 float32 elements.

## Vector Data Types and naming
- `vint32m1_t`: vector of 32-bit integers with LMUL = 1
- `vfloat32m1_t`: vector of 32-bit floats with LMUL = 1

## Vector setting: `vsetvl` / `vsetvli`
Configure vector length and data type using assembly or intrinsics.

### Inline Assembly
Inline Assembly version:
```assembly
asm volatile("vsetvli %0, zero, e32, m8, ta, ma" : "=r"(vl));
```
meaning: 
  - `rd` = destination register (e.g., vl)
  - `rs` = zero register (not dynamic)
  - `e32` = SEW = 32
  - `m8` = LMUL = 8
  - `ta` = tail agnostic
  - `ma` = mask agnostic

### Intrinsic
Intrinsic Version:
```c
size_t vl = vsetvl_e32m1(avl);
```
Returns the vector length for given AVL with SEW = 32 and LMUL = 1.

## Basic Vector Operations
- Loading values:
    ```c
    vfloat32m1_t vin = vle32_v_f32m1(ptr, vl);
    ```
- Storing values:
    ```c
    vse32_v_f32m1(ptr_out, vres, vl);
    ```
- Arithmetic:
    ```c
    vfadd_vv_f32m1 // vector + vector
    vfmul_vf_f32m1 // vector * scalar
    vfsub_vv_f32m1 // vector - vector
    vfmax_vv_f32m1 // max of two vectors
    vfmadd_vv_f32m1//fused multiply-add (y * x + C)
    ```
- Broadcast constants:
    ```c
    vfloat32m1_t one = vfmv_v_f_f32m1(1.0f, vl);
    ```

## Vectorized Exponential Approximation (Taylor-based)
Since RVV cannot use expf() directly, we implement non-linear approximations manually.
Pseudo-code using Taylor N=3:
```c
// f is input vector, T3, T2, T1 are constant coefficient vectors
vfloat32m1_t exp_f = vfmadd_vv_f32m1(f, T3, T2, vl);       // T3*f + T2
exp_f = vfmadd_vv_f32m1(f, exp_f, T1, vl);                 // f*(...) + T1
exp_f = vfmadd_vv_f32m1(f, exp_f, one, vl);                // f*(...) + 1.0
```

## References
- [IBM inline assembly example](https://www.ibm.com/docs/zh-tw/openxl-c-and-cpp-aix/17.1.1?topic=statements-examples-inline-assembly)
- [ara platform repo](https://github.com/pulp-platform/ara)
  - Nonlinear api sample code in ara/apps/...
