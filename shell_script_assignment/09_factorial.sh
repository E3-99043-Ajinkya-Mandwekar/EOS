#!/usr/bin/env bash
read -r -p "Enter a non-negative integer (0 to 20): " n
if ! [[ "$n" =~ ^[0-9]+$ ]] || (( 10#$n > 20 )); then
    echo "Enter an integer from 0 to 20."; exit 1
fi
n=$((10#$n)); fact=1
for ((i=2; i<=n; i++)); do fact=$((fact*i)); done
echo "$n! = $fact"
