#!/usr/bin/env bash
if (( $# != 1 )); then echo "Usage: $0 <filename>"; exit 1; fi
file=$1
if [[ -f "$file" ]]; then
    printf 'Last modification time: '
    stat -c '%y' -- "$file"
else
    echo "File does not exist or is not a regular file: $file"; exit 1
fi
