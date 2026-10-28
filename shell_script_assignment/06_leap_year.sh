#!/usr/bin/env bash
read -r -p "Enter a year: " year
if ! [[ "$year" =~ ^[0-9]+$ ]]; then echo "Enter a valid positive year."; exit 1; fi
if (( year % 400 == 0 || (year % 4 == 0 && year % 100 != 0) )); then
    echo "$year is a leap year."
else
    echo "$year is not a leap year."
fi
