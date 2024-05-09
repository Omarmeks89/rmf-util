#!/bin/bash


echo "===================="
echo "assembly..."

export LD_LIBRARY_PATH="/usr/local/lib"
echo "set path to external lib: ${LD_LIBRARY_PATH}"
make build
if [[ $? -ne 0 ]]; then
  echo "assembly failed..."
  exit 1
fi
echo "done..."
echo "===================="
