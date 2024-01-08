#!/bin/sh

# Log each command and exit on error
set -xe

# Changes to the project root
cd "$(dirname "$(dirname "$(readlink -f "$0" || realpath "$0")")")" || exit 1

# Make sure the Doxygen documentation is up to date
doxygen

date="$(date +'%Y-%m-%d')"
file_name="dist/cs3307-group-project-$date-src.zip"

# Remove the old ZIP file and create a new one
rm "$file_name" || true
mkdir -p 'dist'
zip -r "$file_name" . -i@zip-include-list.txt
