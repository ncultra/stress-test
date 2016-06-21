#!/usr/bin/env bash

# run this program to stress large-file network i/o 
# The server (listener should) be a host that you want to stress
# The client will streem the contents of /dev/urandom over TCP
# to the host. Set $CHILDREN to the number of clients
#
# to run a host as a server:
#
# dcat.sh 0 (from the host's login shell)
# 
# to start 20 clients that will stress the host: 
#
# dcat.sh 20 <host ip>
#
# To kill the clients:
#
# dcat.sh -1 



kill_children() {
    ps --no-headers -C ncat | awk '{print $1}' | xargs kill &> /dev/null
    ps --no-headers -C dd | awk '{print $1}' | xargs kill &> /dev/null
    }


# if $CHILDREN == 0, start a server
# if $CHILDREN == -1, kill all the tests
# if $CHILDREN > zero, start that many ncat clients
CHILDREN=$1

# ip address of the ncat listener. 
SERVER=$2
if   (( $CHILDREN == 0 )) ; then 

    echo "starting ncat server on $(hostname) port 31337"
    sudo ncat -lv --keep-open >/dev/null
    exit 0
fi

if (( $CHILDREN < 0 )) ; then
    kill_children
fi


while (( $CHILDREN > 0 ))
do
    echo "dd-ncat to $SERVER $CHILDREN" 
    dd bs=64536 if=/dev/urandom | sudo ncat $SERVER &
    (( CHILDREN -= 1 ))
done
