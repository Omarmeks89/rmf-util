#!/bin/bash


echo "===================="
echo "assembly..."

make build
if [[ $? -ne 0 ]]; then
  echo "assembly failed..."
  exit 1
fi
echo "done..."
echo "===================="
