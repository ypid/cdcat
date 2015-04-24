#!/bin/sh

for f in $(find . -type f -name '*.ts'); do
    f2=$(echo $f |sed -e 's/ts$/qm/g')
    echo "$0: $f to $f2"
    lrelease -silent -qm "$f2" "$f"
    cp "$f2" ./cdcat.app/Contents/MacOS/lang/
done
