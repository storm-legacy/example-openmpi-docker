#!/bin/sh

# ENVIRONMENT VARIABLES USED:
# $WORKERS_NUM, $SAMPLES_POOL

# Get hosts
HOSTS=$(cut -f1 -d" " hostfile.txt)

# Get ssh keys from each server
echo "" > ~/.ssh/known_hosts
for host in $HOSTS
do
  ssh-keyscan $host >> ~/.ssh/known_hosts 2> /dev/null
done

/usr/local/bin/mpirun -n ${WORKERS_NUM} --hostfile hostfile.txt /app/application ${SAMPLES_POOL}