#!/usr/bin/env python3

import re
from pathlib import Path

RESULT_DIR = Path("/root/E4-Attack-Resistance/results/attack_result")
TOTAL = 1000
pattern = re.compile(r"\bsuccess\b", re.IGNORECASE)

success_count = 0

for i in range(1, TOTAL + 1):
    p = RESULT_DIR / f"result_{i}.txt"
    if not p.is_file():
        print(f"Warning: missing file {p} -> counted as failure")
        continue
    try:
        text = p.read_text(errors="ignore")
    except Exception as e:
        print(f"Warning: failed to read {p}: {e}")
        continue

    if pattern.search(text):
        success_count += 1

ratio = success_count / TOTAL
print(f"BS attack success rate: {success_count} / {TOTAL} = {ratio}")
