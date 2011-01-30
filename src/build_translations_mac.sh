#!/bin/sh
files=$(find . -type f -name '*.ts')
for f in $files; do
    f2=$(echo $f |sed -e 's/ts$/qm/g')
    echo "$0: $f to $f2"
    lrelease -silent -qm $f2 $f
done
