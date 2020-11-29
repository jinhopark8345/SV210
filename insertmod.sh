#!/bin/bash

for module in `find ./ -maxdepth 1 -iname '*.ko'`
do
    echo "insert module: $module"
    insmod $module
done

echo "currently inserted modules:"
lsmod
