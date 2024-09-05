#!/usr/bin/env bash
# Usage: ./scatter.sh bin_path ip-or-hostname1:path/to/folder ip-or-hostname2:path/to/folder...

if [ "$#" -eq 0 ]
then
  echo "Must specify targets to scatter files." >&2
  exit 1
fi

trap "exit" INT

NODES=($@)
M=${#NODES[@]}
BIN_PATH=$1
BIN_PATH=`realpath ${BIN_PATH}`

for i in $(seq 1 $(($M-1)))
do
  target="ubuntu@${NODES[$i]}:/mnt/fix/bins"
  source=${BIN_PATH}/bin$((i-1))
  echo "Sending $source to $target."
  echo "rsync -a --delete $source/.fix/ $target/.fix/"
  rsync -a --delete $source/.fix/ $target/.fix/ -y
done
