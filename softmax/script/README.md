# Script Utilities for Softmax Input Data

This folder contains Python utilities for generating and visualizing softmax input data.

## Requirements
Ensure you have Python 3 installed
- Recommended version: Python 3.10.12
- Required packages (With recommeded version) :
  - matplotlib 3.10.5
  - numpy 2.2.6

Install required packages:
```bash
pip3 install matplotlib==3.10.5 numpy==2.2.6
```

## Files and Usage
- `gen_data.py`
Generates a .S file with floating-point inputs for softmax.

Usage
```bash
python3 gen_data.py [size] [start] [end] > data.S
```
  - size: Number of data points
  - start: Minimum float value
  - end: Maximum float value
```

- `plot_data.py`
Visualizes the distribution of generated values as a histogram.

Usage
```bash
python3 plot_data.py input.txt
```
Where input.txt is a plain-text file containing one float per line.

## Example
```bash
python3 gen_data.py 512 0 10 > data.S
python3 plot_data.py input.txt
```
