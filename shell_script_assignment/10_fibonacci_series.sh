#!/usr/bin/env bash
read -r -p "How many Fibonacci terms? (1 to 90): " terms
if ! [[ "$terms" =~ ^[0-9]+$ ]] || (( 10#$terms < 1 || 10#$terms > 90 )); then
    echo "Enter a number of terms from 1 to 90."; exit 1
fi
terms=$((10#$terms)); a=0; b=1
for ((i=1; i<=terms; i++)); do
    printf '%s' "$a"
    if (( i < terms )); then printf ' '; fi
    next=$((a+b)); a=$b; b=$next
done
printf '\n'
