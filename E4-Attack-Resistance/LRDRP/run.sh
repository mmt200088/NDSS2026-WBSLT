#!/usr/bin/env bash

set -u

TOTAL=1000

mkdir -p ../results/LRDRP-result
cd ./WBSLT-optimized
mkdir build
cd build
cmake ..
make

for i in $(seq 1 1000); do
  outfile="../../../results/LRDRP-result/attack_result_${i}.txt"  
  echo "LRDRP attack ${i} / ${TOTAL}"
  ./WBAES >> "$outfile" 2>&1
done

