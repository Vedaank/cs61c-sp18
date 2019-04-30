#!/bin/bash

# Just run this file and you can test your circ files!
# Make sure your files are in the directory above this one though!

cp mem.circ tests/cpu_single
cp alu.circ tests/cpu_single
cp regfile.circ tests/cpu_single
cp cpu.circ tests/cpu_single
cd tests/cpu_single
./sanity_test.py
cd ../..
