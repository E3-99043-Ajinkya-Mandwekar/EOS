#!/usr/bin/env bash
# Source this script from ~/.bashrc to run it in each interactive Bash shell.
printf '\nWelcome, %s!\n' "$(whoami)"
printf 'Your home directory (%s) contains:\n' "$HOME"
ls -la -- "$HOME"
