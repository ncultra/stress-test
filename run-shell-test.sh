#!/usr/bin/bash

# run-shell-test <children> command-line
# 

CHILDREN=$1
shift
while (( $CHILDREN > 0))
do
    $($* &>/dev/null ) & 
    (( CHILDREN -= 1 ))
done
echo "waiting"
wait
