#!/usr/bin/env bash

SERVER=0
HOST_ADDR=127.0.0.1
HOST_PORT=31337
NUM_CLIENTS=0
CLIENT_ADDR=127.0.0.1
CLIENT_PORT=59000
UDP=""
KILL_CLIENTS=0
CAT_FILE=""
REPEAT=1

function kill_children
{
    ps --no-headers -C ncat | awk '{print $1}' | xargs kill &> /dev/null
    if (( ${#CAT_FILE} > 0 )) ; then
	ps --no-headers -C cat | awk '{print $1}' | xargs kill &> /dev/null
    else
	ps --no-headers -C dd | awk '{print $1}' | xargs kill &> /dev/null
    fi
}

function cat_file()
{
    echo "cat $CAT_FILE to $HOST_ADDR:$HOST_PORT"
    while (( $REPEAT > 0 ))
    do
	cat $CAT_FILE 2>"/dev/null" | \
	    ncat $UDP $HOST_ADDR $HOST_PORT -s $SOURCE_ADDR -p $SOURCE_PORT  2>"/dev/null" &
	(( REPEAT -= 1 ))
    done
}

function dd_file()
{
    echo "dd /dev/urandom to $HOST_ADDR:$HOST_PORT"
    dd bs=64536 count=$REPEAT if=/dev/urandom 2>"/dev/null" | \
	ncat $UDP $HOST_ADDR $HOST_PORT -s $SOURCE_ADDR -p $SOURCE_PORT 2>"/dev/null" &
}

function start_clients()
{
    while (( $NUM_CLIENTS > 0 ))
    do
	if (( ${#CAT_FILE} > 0 )) ; then
	    cat_file
	else
	    dd_file
	fi
	(( NUM_CLIENTS -= 1 ))
	(( SOURCE_PORT += 1 ))
    done
}

function start_server()
{
    echo "running in server mode (SERVER = $HOST_ADDR:$HOST_PORT)"
    ncat $UDP -lv -m 1000 --keep-open $HOST_ADDR $HOST_PORT &>/dev/null
}


function usage()
{
    echo "`basename $0` <options>"
    echo " -s start as a server (keep listening and fork)"
    echo " -h <host> listener ip address"
    echo " -p <port> listener port"
    echo " -c <num> start <num> clients"
    echo " -b <client> client ip address"
    echo " -e <port> ephemeral (source) client port (each subsequent client increments)"
    echo " -u use udp"
    echo " -k kill clients"
    echo " -f <filename> cat filename over ncat"
    echo " -r <repeat> number of times to cat file, or number of blocks to dd"
    exit 1
}


if (( $# == 0 )) ; then
    usage
fi

while getopts "sh:p:c:b:e:ukf:r:" opt
do
    case $opt in
	s) SERVER=1 ;;
	h) HOST_ADDR=$OPTARG; echo "host address is $HOST_ADDR" ;;
	p) HOST_PORT=$OPTARG; echo "host port is $HOST_PORT" ;;
        c) NUM_CLIENTS=$OPTARG; echo "starting $NUM_CLIENTS clients" ;;
	b) CLIENT_ADDR=$OPTARG; echo "client binding to $CLIENT_ADDR" ;;
	e) CLIENT_PORT=$OPTARG; echo "client ephemeral port is $CLIENT_PORT" ;;
	u) UDP="--udp"; echo "using UDP" ;;
	k) KILL_CLIENTS=1; echo "Killing $NUM_CLIENTS clients" ;;
	f) CAT_FILE=$OPTARG; echo "transferring file $CAT_FILE" ;;
	r) REPEAT=$OPTARG; echo "$REPEAT repeats/blocks" ;;
    esac
done


if (( $SERVER > 0 )) ; then
    start_server
else
    start_clients
fi
