#!/bin/bash

echo "===================="
echo "uninstall..."
sudo make clear
echo "remove user..."
sudo userdel -r rmf
echo "done..."
echo "===================="

