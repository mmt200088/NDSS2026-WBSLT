#!/usr/bin/env bash

set -euo pipefail

TOTAL=63

cd ./HO-DCA-main/data_dependency_on_SEL_masking/d_1
mkdir -p build
cd build
cmake ..
make

mkdir -p /root/E4-Attack-Resistance/results/DCA-result

if [[ ! -x ./WBAES ]]; then
  echo "Error: ./WBAES not found or not executable in $(pwd)" >&2
  exit 2
fi

for i in $(seq 1 63); do
  outfile="/root/E4-Attack-Resistance/results/DCA-result/attack_result_${i}.txt"
  echo "DCA attack ${i} / ${TOTAL}"
  ./WBAES > "$outfile" 2>&1
done