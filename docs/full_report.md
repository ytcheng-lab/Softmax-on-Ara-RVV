# 1. Summary
This project evaluates the acceleration of softmax using RVV vector intrinsics. The result shows up to 99% execution time can be reduced by replacing glibc’s scalar expf() with a Taylor-approximated SIMD version.

# 2. Motivation
Found nonlinear function bottleneck for AI workload. e.g. MobileBERT computing bottleneck: softmax
During the research of reducing softmax execution time, tried CVA6 + ARA1.0 platform to compare scalar/vector efficiency.

# 3. Target Platform and Toolchain
Use pulp series ARA platform (CVA6 + ARA 1.0) as benchmark.
- Toolchain
  - LLVM 16.0.0
- Simulator
  - Verilator 4.0
- CVA6
  - RV32GC
- Ara vector unit setting
  - VLEN=4096
  - LMUL=1
  - SEW=32

# 4. RVV Intrinsics and Nonlinear Approximation
Some key points:
- No LUT support in RVV. This explains why Ara’s exponential API builds on Cephes-like strategies, which rely on shifts and multiplications instead of look-up tables.
Writing RVV-friendly nonlinear functions comes at a cost. The required instruction footprint can be large — which is why my current team prefers in-house vector ISA extensions tailored for critical workloads.
- RVV enables significant acceleration, but only when you re-implement non-linear functions like exp(x) using vectorized approximations.

More details about intrinsic funciton usages refer to: [RVV beginner note](RVV_beginner_note.md)

# 5. Implementation Details

- `softmax`
- `softmax_vec3`
- `softmax_vec5`

# 6. Experimental Setup
Simulation platform
Input size: 512 single-precision floats, range [0, 10)
- Baseline measurement:
  - CVA6 with standard glibc exp() function called on all inputs.
  - Measures mcycle before and after for total dynamic cycle count.
- Optimized measurement:
  - 3rd-degree Taylor approximation with RVV inplemented.
  - 5th-degree Taylor approximation with RVV inplemented.

# 7. Results and Analysis
| Method                | Cycle Count | Time Reduction |
| --------------------- | ----------- | -------------- |
| Scalar (glibc `expf`) | 124,887     | 0%             |
| Taylor N=3 no RVV     | 14,644      | 88%            |
| Taylor N=3 with RVV   | 1,598       | 99%            |

# 8. Discussion
- RVV intrinsics have a steep learning curve.
- RVV enables significant acceleration, but only when you re-implement non-linear functions like exp(x) using vectorized approximations.
- No LUT support in RVV. This explains why Ara’s exponential API builds on Cephes-like strategies, which rely on shifts and multiplications instead of look-up tables.
- Writing RVV-friendly nonlinear functions comes at a cost. The required instruction footprint can be large — which is why my current team prefers in-house vector ISA extensions tailored for critical workloads.

# 9. Conclusion
- RVV reduce execution time of softmax up to 99% with implement of nonlinear function with RVV intrinsics

# 10. future work: 
- Docker environment for ARA testing
- More nonlinear functions implement: GELU, tanh ... .etc

# 10. Reference
- [RVV beginner guide](RVV_beginner_note.md)
- [ARA platform](https://github.com/pulp-platform/ara)
- [CVA6](https://github.com/openhwgroup/cva6)
