#!/usr/bin/env bash
read -r -p "Enter a number: " n
if ! [[ "$n" =~ ^-?[0-9]+$ ]]; then echo "Invalid integer."; exit 1; fi
for ((i=1; i<=10; i++)); do printf '%d x %d = %d\n' "$n" "$i" "$((n*i))"; done
