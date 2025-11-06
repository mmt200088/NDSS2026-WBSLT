#!/usr/bin/env bash

set -u
TOTAL=1000
cd ./DarkPhoenix-main

mkdir -p /root/E4-Attack-Resistance/results/DFA-result

for i in $(seq 1 1000); do
  outfile="/root/E4-Attack-Resistance/results/DFA-result/attack_result_${i}.txt"
  echo "DFA attack ${i} / ${TOTAL}"
  python3 -m darkphoenixAES > "$outfile" 2>&1
done
