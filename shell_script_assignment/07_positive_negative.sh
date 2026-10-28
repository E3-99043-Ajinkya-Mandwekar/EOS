#!/usr/bin/env bash
read -r -p "Enter an integer: " n
if ! [[ "$n" =~ ^-?[0-9]+$ ]]; then echo "Invalid integer."; exit 1; fi
if (( n > 0 )); then echo "$n is positive."
elif (( n < 0 )); then echo "$n is negative."
else echo "The number is zero (neither positive nor negative)."
fi
