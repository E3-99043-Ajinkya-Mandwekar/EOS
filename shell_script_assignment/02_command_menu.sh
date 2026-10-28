#!/usr/bin/env bash
while true; do
    echo "1. Date"
    echo "2. Cal"
    echo "3. Ls"
    echo "4. Pwd"
    echo "5. Exit"
    read -r -p "Enter your choice: " choice
    case "$choice" in
        1) date ;;
        2) if command -v cal >/dev/null 2>&1; then cal; else echo "'cal' is not installed."; fi ;;
        3) ls ;;
        4) pwd ;;
        5) echo "Exiting."; break ;;
        *) echo "Invalid choice." ;;
    esac
    echo
done
