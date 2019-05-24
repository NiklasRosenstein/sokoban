#!/bin/bash

INTERVAL=120

while (true); do
    ctags -f tags -R --sort=yes --extra=+f --fields=+sS src/
    echo -e "`date +"%F %H:%M"`\t\tGenerated ctags"
    sleep $INTERVAL
done

