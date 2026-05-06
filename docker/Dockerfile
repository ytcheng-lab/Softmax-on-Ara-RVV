FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    vim \
    build-essential \
    cmake \
    clang \
    python3 \
    python3-pip \
    device-tree-compiler \
    libyaml-dev \
    help2man \
    flex \
    bison \
    libelf-dev \
    autoconf \
    gperf \
    curl \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install numpy

WORKDIR /ara

RUN git clone --recursive https://github.com/pulp-platform/ara.git .

RUN \
    sed -i '/cmake/ s/$/ -DCOMPILER_RT_BUILD_CTX_PROFILE=OFF/' Makefile && \
    \
    sed -i '/CC=$(CLANG_CC) CXX=$(CLANG_CXX)/d' Makefile && \
    \
    sed -i 's/make -j$(nproc)/make/g' Makefile

RUN make toolchain-llvm -j$(nproc)

RUN make riscv-isa-sim -j$(nproc)

RUN make verilator

RUN mkdir -p /ara/install/verilator/share/verilator && \
    ln -sf /ara/install/verilator/bin/verilator_bin /ara/install/verilator/share/verilator/verilator_bin

ENV PATH="/ara/install/bin:${PATH}"

CMD ["/bin/bash"]
