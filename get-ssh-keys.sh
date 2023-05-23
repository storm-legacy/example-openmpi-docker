#!/bin/sh

# Get hosts
HOSTS=$(cut -f1 -d" " hostfile.txt)

# Get ssh keys from each server
echo "" > ~/.ssh/known_hosts
for host in $HOSTS
do
  ssh-keyscan $host >> ~/.ssh/known_hosts
done