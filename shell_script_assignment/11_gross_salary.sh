#!/usr/bin/env bash
read -r -p "Enter basic salary: " basic
if ! [[ "$basic" =~ ^[0-9]+([.][0-9]+)?$ ]]; then
    echo "Enter a non-negative salary, e.g. 25000 or 25000.50."; exit 1
fi
awk -v basic="$basic" 'BEGIN {
    da=basic*0.40; hra=basic*0.20; gross=basic+da+hra
    printf "Basic salary: %.2f\nDA (40%%): %.2f\nHRA (20%%): %.2f\nGross salary: %.2f\n", basic, da, hra, gross
}'
