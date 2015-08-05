#!/bin/bash

# Install sdsl
./external/sdsl-lite/install.sh

# Build perf
mkdir -p build
cd build
cmake ../
START=$(date +%s)
make
END=$(date +%s)
echo "Total Build time (real) = $(( $END - $START )) seconds"
