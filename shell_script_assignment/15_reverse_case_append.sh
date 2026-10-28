#!/usr/bin/env bash
read -r -p "Enter source filename: " source
read -r -p "Enter destination filename: " destination
if [[ ! -f "$source" || ! -r "$source" ]]; then echo "Source file is missing or unreadable."; exit 1; fi
if [[ -e "$destination" && "$source" -ef "$destination" ]]; then
    echo "Source and destination must be different files."; exit 1
fi
if [[ ! -e "$destination" ]]; then touch -- "$destination" || exit 1
elif [[ ! -f "$destination" || ! -w "$destination" ]]; then
    echo "Destination must be a writable regular file."; exit 1
fi
tr '[:upper:][:lower:]' '[:lower:][:upper:]' < "$source" >> "$destination"
echo "Case-swapped content appended to $destination."
