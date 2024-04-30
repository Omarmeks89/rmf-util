#!/bin/bash

echo "===================="
sudo chown root rmf
if [[ $? -ne 0 ]]; then
  echo "not allowed... Try as sudoer"
  exit 1
else
  sudo chmod +s rmf
  if [[ $? -ne 0 ]]; then
    echo "<setuid> not set..."
    exit 1
  fi
fi

echo "create user..."
sudo adduser --system --group --home "/var/lib/rmf" --debug rmf
if [[ $! -eq 0 ]]; then
  sudo usermod -aG rmf rmf
  if [[ $? -ne 0 ]]; then
    echo "user not added to group..."
    exit 1
  fi
else
  echo "user and group not created..."
  exit 1
fi

echo "done..."
echo "===================="

