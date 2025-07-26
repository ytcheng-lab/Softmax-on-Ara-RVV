# Softmax-on-Ara-RVV

## 📌 Summary
> **This project benchmarks a vectorized softmax implementation on the Ara platform (CVA6 + RVV), using custom RVV intrinsics and Spike simulation to evaluate acceleration benefits over scalar RISC-V execution.**

## Motivation
While implementing MobileBERT on RISC-V cores, softmax became a performance bottleneck due to its reliance on `exp(x)` and vector reductions.  
This project explores how the Ara platform, equipped with a RISC-V Vector Extension (RVV 1.0) coprocessor, can accelerate softmax via data-level parallelism.

I wrote a standalone RVV-based softmax kernel and integrated it into Ara’s `apps/` folder to:
- Simulate it using Ara’s Spike backend
- Measure execution cycles via `mcycle`
- Compare it against scalar (RV32IM) and software-optimized versions

## Evaluation Method
This experiment was conducted by:
- Writing RVV softmax code using intrinsics (`vsetvl`, `vle32.v`, `vfadd.vv`, etc.)
- Building and running it with Ara’s Spike setup
- Recording `mcycle` count for different input sizes and VLEN settings

The measurement includes:
- Instruction-level simulation accuracy
- Static vector lengths with up to 8 lanes
- Comparison with scalar softmax (RV32IM)

## Result Summary
(tbd)
| Configuration         | Input Size | VLEN | Lanes | Cycles | Speedup |
|-----------------------|------------|------|--------|--------|---------|
| RV32IM Scalar         |            |      |        |        |         |
| RV32IM + SW Opt (LUT) |            |      |        |        |         |
| Ara (no RVV) Softmax  |            |      |        |        |         |
| Ara RVV Softmax       |            |      |        |        |         |

## Repo Structure
- softmax-on-ara-rvv/
  - README.md
  - rvv_softmax_demo/
    - main.c               # RVV softmax kernel
    - Makefile             # For Ara/apps usage
    - perf_log.txt         # Spike output log
  - docs/
    - perf-summary.md      # Result writeup
    - vector_strategy.md   # RVV mapping explanation

## How to Use with Ara
This repo is designed to plug into the official Ara repository.  
To test the demo:
```bash
# Clone Ara with submodules
git clone --recursive https://github.com/pulp-platform/ara.git
cd ara
# Copy demo into apps/
cp -r ../softmax-on-ara-rvv/rvv_softmax_demo apps/
# Build and simulate
cd apps
make bin/rvv_softmax_demo.spike
make spike-run-rvv_softmax_demo
```

## Next Steps
- Evaluate performance across different VLEN and lane settings
- Try Ideal Dispatcher mode for theoretical peak throughput
- Expand to vectorized GELU, tanh, or layer norm kernels
- Integrate result into overall MobileBERT pipeline

## Reference
- [ara platform](https://github.com/pulp-platform/ara)
