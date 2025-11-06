#!/usr/bin/env python3

import re
from pathlib import Path

RESULT_DIR = Path("/root/E4-Attack-Resistance/results/DFA-result")
TOTAL = 1000
pattern = re.compile(r"\bsuccess\b", re.IGNORECASE)

success_count = 0

for i in range(1, TOTAL + 1):
    p = RESULT_DIR / f"attack_result_{i}.txt"
    if not p.is_file():
        print(f"Warning: missing file {p} -> counted as zero")
        continue
    try:
        text = p.read_text(errors="ignore")
    except Exception as e:
        print(f"Warning: failed to read {p}: {e}")
        continue

    matches = pattern.findall(text)
    success_count += len(matches)

ratio = success_count / TOTAL
print(f"DFA attack success rate: {success_count} / {TOTAL} = {ratio}")
