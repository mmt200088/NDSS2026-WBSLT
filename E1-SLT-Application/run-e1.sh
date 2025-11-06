#!/bin/bash

mkdir results

# Navigate to WBSLT-SLT directory
cd ./WBSLT-SLT

# Create a build directory and navigate into it
mkdir -p build
cd build

# Run cmake and make to build the project
cmake ..
make

# Define the output file
OUTPUT_FILE="../../results/data.txt"

# Enumerate the values for -r (from 6 to 32) and -n (64 and 128)
for r in $(seq 6 32); do
    for n in 64 128; do
        # Run the executable with the given -r and -n
        ./WBAES -r $r -n $n >> "$OUTPUT_FILE"
    done
done

SHARK_FILE="../../results/shark-data.txt"
./WBAES -r 6 -n 64 >> "$SHARK_FILE"

PRESENT_FILE="../../results/present-data.txt"
./WBAES -r 30 -n 64 >> "$PRESENT_FILE"
