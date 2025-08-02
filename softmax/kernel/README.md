# Softmax Kernel Implementation
This folder contains different implementations of the softmax function used for benchmarking on the Ara platform (CVA6 + RVV 1.0).

## Summary
This kernel explores how RVV vector intrinsics can accelerate softmax execution by approximating the exponential function with polynomial expansion. Since glibc does not support vectorized operations, all vector-based softmax variants use manual non-linear approximation and RVV intrinsic operations (e.g., vfadd.vv, vfmul.vf).

For background on RVV intrinsics, refer to `docs/RVV_beginner_note.md` (coming soon)

## File Overview
- softmax.c: Function implementations for scalar and RVV-based softmax
- softmax.h: Header file for softmax function declarations

## Function List
- `softmax`
  - Scalar baseline model using glibc’s expf()
  - No RVV vectorization involved
  - Used as the baseline for comparison

- `softmax_vec5`
  - Uses Taylor order-5 approximation for exp(x)
  - Fully vectorized using RVV intrinsics

- `softmax_vec3`
  - Uses Taylor order-3 approximation for exp(x)
  - Also fully vectorized using RVV intrinsics
  - Slightly lower accuracy, but faster computation compared to vec5

## Vector Configuration
These implementations assume the following Ara RVV configuration:
- VLEN: 4096 bits
- SEW: 32 bits
- LMUL: 1
- Maximum AVL: 128
