# Softmax-on-Ara-RVV

## 📌Summary
> This project evaluates the acceleration of softmax using RVV vector intrinsics. The result shows up to 99% execution time can be reduced by replacing glibc’s scalar expf() with a Taylor-approximated SIMD version.

## Motivation
While porting MobileBERT to RISC-V, we identified softmax as a key bottleneck due to:
- Heavy reliance on exp(x) and reduction ops
- Lack of glibc support for vectorized math
This project investigates how vector intrinsics can enable nonlinear acceleration via RVV, and evaluates the trade-off between accuracy and execution time.

## Evaluation Method
Implemented scalar and RVV-based softmax variants:
 - softmax scalar, glibc expf() (baseline)
 - softmax Taylor order-3, no RVV
 - softmax Taylor order-3, RVV

### Environment
- Platform: Ara (CVA6 + RVV 1.0)
- Simulation tool: Verilator

## Result Summary
Used Verilator to simulate real mcycle counts.
Settings:
Input: 512 float32 values, range [0, 10)
**Results**
|Method|Cycle Count|Time Reduction|
|------|-----------|--------------|
|Scalar (glibc expf)|124,887|0%|
|Taylor N=3 no RVV|14,644|88%|
|Taylor N=3 with RVV|1,598|99%

***Observations***
- RVV brings no benefit unless the softmax kernel is rewritten using vector intrinsics
- With a proper SIMD-friendly exp(x) approximation (e.g., Taylor expansion), RVV drastically reduces latency
- Approximation degree can tune the speed/accuracy trade-off for deployment

## Repo Structure

- Softmax-on-Ara-RVV/
  - softmax/
    - main.c
    - Makefile
    - kernel/             # softmax.c / softmax.h
    - script/             # gen_data.py / plot_data.py
  -  docs/
    - full_report.md
    - RVV_beginner_note.md
  - README.md

## How to Use with Ara (CVA6 + RVV)
1. Clone the Ara repo with submodules:
```bash
git clone --recursive https://github.com/pulp-platform/ara.git
```

2. Copy this project’s softmax/ folder into apps/:
```bash
cd ara
cp -r ../Softmax-on-Ara-RVV/softmax ara/apps
```
3. Build and run with Verilator:
```
cd apps
make bin/softmax
cd hardware
make sim app=softmax
```

## Future Work
- Benchmark with other VLEN/LMUL/AVL configurations
- Add support for GELU, tanh, and layer norm kernels
- Integrate softmax into full MobileBERT workload
- Evaluate error impact from Taylor approximation vs expf()

## Reference
- [ara platform repo](https://github.com/pulp-platform/ara)
- [LLVM RVV Intrinsics Guide](doc/RVV_beginner_note.md)


