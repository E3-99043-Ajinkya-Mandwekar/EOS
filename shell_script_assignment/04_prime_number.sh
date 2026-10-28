#!/usr/bin/env bash
read -r -p "Enter an integer: " input
if ! [[ "$input" =~ ^-?[0-9]+$ ]]; then echo "Invalid integer."; exit 1; fi
n=$((10#$input))
if (( n < 2 )); then echo "$n is not prime."; exit 0; fi
is_prime=1
for ((i=2; i*i<=n; i++)); do
    if (( n % i == 0 )); then is_prime=0; break; fi
done
if (( is_prime )); then echo "$n is prime."; else echo "$n is not prime."; fi
