#!/usr/bin/env bash
shopt -s nullglob dotglob
entries=(.*); found=0
for entry in "${entries[@]}"; do
    [[ "$entry" == "." || "$entry" == ".." ]] && continue
    printf '%s\n' "$entry"; found=1
done
if (( ! found )); then echo "No hidden files or directories found."; fi
