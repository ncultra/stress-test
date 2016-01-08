#!/usr/bin/bash

CHILDREN=100

while (( $CHILDREN > 0))
do
    dd if=/dev/random | bzip2 -1 >/dev/null & 
    
    (( CHILDREN -= 1 ))
done

echo "waiting"
wait
killall dd

