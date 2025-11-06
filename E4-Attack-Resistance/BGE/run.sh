#!/usr/bin/env bash

set -u

GEN_DIR="/root/E4-Attack-Resistance/BGE/WBAC-Resistance-Algorithm-Generator/gen"
BGE_SRC="/root/E4-Attack-Resistance/BGE/BlueGalaxyEnergy/src"
BGE_SUBDIR="${BGE_SRC}/bluegalaxyenergy"
ROOT_LOG_DIR="/root/E4-Attack-Resistance/results/BGE-log"
TOTAL=1000

timestamp() { date +"%Y-%m-%d %H:%M:%S"; }

for (( x=1; x<=TOTAL; x++ )); do
  LOG_DIR="${ROOT_LOG_DIR}/log${x}"
  ATTACK_LOG="${LOG_DIR}/attack_log${x}.txt"

  mkdir -p "${LOG_DIR}" || {
    ATTACK_LOG="/tmp/attack_log${x}.txt"
  }

  if [ -d "${GEN_DIR}" ]; then
    {
      cd "${GEN_DIR}" && \
      make table_gen && \
      ./table_gen
    } >> "${ATTACK_LOG}" 2>&1
  fi

  if [ -d "${BGE_SRC}" ]; then
    {
      cd "${BGE_SRC}" && \
      echo "BGE attack ${x} / ${TOTAL}"
      python3 -m bluegalaxyenergy > "${ATTACK_LOG}" 2>&1
    }
  fi
done
