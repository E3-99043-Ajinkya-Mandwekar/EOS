#!/usr/bin/env bash
shopt -s nullglob
found=0
for entry in *; do
    if [[ -f "$entry" && -x "$entry" ]]; then printf '%s\n' "$entry"; found=1; fi
done
if (( ! found )); then echo "No executable files found in the current directory."; fi
