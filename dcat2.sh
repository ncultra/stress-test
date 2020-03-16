#!/usr/bin/env bash

# -s start as a server (keep listening)
# -h host (listener ip address)
# -p port (listener port)
# -c <num> start <num> clients
# -e ephemeral client port (each subsequent client increments)
# -t use tcp
# -u use udp
# -k kill clients
# -f cat file (default is /dev/urandom)

SERVER=0
HOST_ADDR=""
HOST_PORT=""
NUM_CLIENTS=1
EPHEMERAL_PORT=59000
TCP=1
UDP=0
KILL_CLIENTS=0
CAT_FILE="/dev/urandom"

function usage()
{
    echo "`basename $0` <options>"
    echo " -s start as a server (keep listening)"
    echo " -h host (listener ip address)"
    echo " -p port (listener port)"
    echo " -c <num> start <num> clients"
    echo " -e ephemeral client port (each subsequent client increments)"
    echo " -t use tcp"
    echo " -u use udp"
    echo " -k kill clients"
    echo " -f cat file (default is /dev/urandom)"
    exit 1
}

if (( $# == 0 )) ; then
    usage
fi

while getopts "sh:p:c:e:tukf:" opt
do
    case $opt in
	s) SERVER=1; echo "running in server mode (SERVER = $SERVER)" ;;
	h) HOST_ADDR=$OPTARG; echo "host address is $HOST_ADDR" ;;
	p) HOST_PORT=$OPTARG; echo "host port is $HOST_PORT" ;;
        c) NUM_CLIENTS=$OPTARG; echo "starting $NUM_CLIENTS clients" ;;
	e) EPHEMERAL_PORT=$OPTARG; echo "client ephemeral port is $EPHEMERAL_PORT" ;;
	t) TCP=1; echo "using TCP" ;;
	u) UDP=1; echo "using UDP" ;;
	k) KILL_CLIENTS=1; echo "Killing $NUM_CLIENTS clients" ;;



    esac
done
