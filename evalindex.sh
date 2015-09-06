#!/bin/sh

if [ $1 = "-v" ]; then

index=$2
descriptors=$3
./evalindex_xsci --features $index $descriptors 

else

index=$1
descriptors=$2

./evalindex_xsci --features $index $descriptors | tail -n 1 | cut -f1 -d" "


fi
