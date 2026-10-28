#!/usr/bin/env bash
read -r -p "Enter a file or directory path: " path
if [[ -f "$path" ]]; then
    echo "It is a file."
    stat -c 'Size: %s bytes' -- "$path" 2>/dev/null || wc -c < "$path"
elif [[ -d "$path" ]]; then
    echo "It is a directory. Contents:"
    ls -la -- "$path"
elif [[ -e "$path" ]]; then
    echo "The path exists, but is neither a regular file nor a directory."
else
    echo "No such file or directory: $path"
fi
