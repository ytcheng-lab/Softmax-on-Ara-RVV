#!/usr/bin/env python3

import random as rand
import numpy as np
import sys

def emit(name, array, alignment='8'):
  print(".global %s" % name)
  print(".balign " + alignment)
  print("%s:" % name)
  bs = array.tobytes()
  for i in range(0, len(bs), 4):
    s = ""
    for n in range(4):
      s += "%02x" % bs[i+3-n]
    print("    .word 0x%s" % s)

def rand_matrix(N, dtype):
  return np.random.rand(N).astype(dtype)

############
## SCRIPT ##
############

if len(sys.argv) == 4:
  N = int(sys.argv[1])
  R1 = int(sys.argv[2])
  R2 = int(sys.argv[3])
else:
  print("Error. Give 3 arguments: gen_data.py [size] [start] [end]")
  sys.exit()

# Input x array
in_matrix = rand_matrix(N, np.float32) * (R2-R1) + R1;
np.savetxt('input.txt', in_matrix);

# Output array; o_g for golden and o_t for test
o_g = np.zeros(N, dtype=np.float32)
o_t = np.zeros(N, dtype=np.float32)
buf = np.zeros(N, dtype=np.float32)

# Create the file
print(".section .data,\"aw\",@progbits")
emit("N", np.array(N, dtype=np.int32))
emit("i", in_matrix)
emit("o_g", o_g)
emit("o_t", o_t)
emit("buf", buf)
