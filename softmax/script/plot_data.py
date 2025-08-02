#!/usr/bin/env python3

# Usage: python3 plot.py input.txt
# This program will plot data in input.txt

import matplotlib.pyplot as plt
import numpy as np
import sys

fname = sys.argv[1]
x = [];
f = open(fname,'r')
for line in f.readlines():
    x.append(float(line))
f.close()

plt.hist(x);
plt.show();
