#!/usr/bin/env bash

BASE="/root/E4-Attack-Resistance/BS/attack/AES/cryptanalysis/wbslt"
RESULT_DIR="/root/E4-Attack-Resistance/results/attack_result"

mkdir -p "$RESULT_DIR"

TOTAL=1000

cd "$BASE" || { echo "ERROR: cannot cd to $BASE"; exit 1; }

i=1
while [ $i -le 1000 ]; do
  echo "BS attack ${i} / ${TOTAL}"
  go test > "${RESULT_DIR}/result_${i}.txt" 2>&1
  i=$((i + 1))
done
