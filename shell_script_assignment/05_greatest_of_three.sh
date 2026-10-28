#!/usr/bin/env bash
read -r -p "Enter three integers separated by spaces: " a b c
if ! [[ "$a" =~ ^-?[0-9]+$ && "$b" =~ ^-?[0-9]+$ && "$c" =~ ^-?[0-9]+$ ]]; then
    echo "Enter three valid integers."; exit 1
fi
if (( a >= b && a >= c )); then greatest=$a
elif (( b >= a && b >= c )); then greatest=$b
else greatest=$c
fi
echo "Greatest number: $greatest"
