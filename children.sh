#!/usr/bin/env bash

# this file intended to be sourced by an executable script

# $1 is the pid to store
record_child_pid() {
    f=$(mktemp "$0XXXX.pid")
    echo "$1" > $f
}


remove_record() {

    for file in $0*.pid ; do
	cat $file
	rm $file
    done
}


#$(ls &>/dev/null) &
#childpid=$!
#record_child_pid $childpid
#sleep 2
#remove_record
