#!/usr/bin/bash

CHILDREN=$1
shift
while (( $CHILDREN > 0))
do
#    dd if=/dev/random | bzip2 -1 >/dev/null & 
    $($* &>/dev/null ) & 
    (( CHILDREN -= 1 ))
done
echo "waiting"
wait
#killall dd
killall $1
