#!/usr/bin/env bash

# run this program to stress large-file network i/o
# The server (listener should) be a host that you want to stress
# The client will streem the contents of /dev/urandom over TCP
# to the host. Set $CHILDREN to the number of clients
#
# to run a host as a server:
#
# dcat.sh 0 <ip> <udp | tcp> (from the host's login shell)
#
# to start 20 clients that will stress the host:
#
# dcat.sh 20 <host ip> <udp | tcp>
#
# To kill the clients:
#
# dcat.sh -1
#
#


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

# protocol and block size
PROT=$3

COUNT=$4
echo "$PROT"
if   (( $CHILDREN == 0 )) ; then
	echo "starting ncat server on $SERVER::31337"
	sudo ncat -lv -m 1000 --keep-open $SERVER 31337 &>/dev/null &
	exit 0
fi

if (( $CHILDREN < 0 )) ; then
    kill_children
fi


while (( $CHILDREN > 0 ))
do
    if [ $PROT = "ip" ] ; then
	echo "dd-ncat over ip to $SERVER $CHILDREN"
	dd bs=64536 count=$4 if=/dev/urandom 2>/dev/null | sudo ncat $SERVER 31337  &
    else
	echo "dd-ncat over udp to $SERVER $CHILDREN"
	dd bs=256 count=$4 if=/dev/urandom 2>/dev/null | sudo ncat --udp $SERVER 31337 &
    fi
    (( CHILDREN -= 1 ))
done

