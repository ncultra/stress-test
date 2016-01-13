#!/usr/bin/env bash


# $1 == how many seconds to sleep before destroying domains
# $2 == number of domains to start/stop


# bash generate random 32 character alphanumeric string (lowercase only)


if (( $EUID )) ; then
echo "you need root privileges"
    exit 1
fi

destroy_minios() {
    for el in ${domains[@]} ; do
	xl destroy $el
    done
}


trap 'destroy_minios; echo "caught a signal, exiting..."; exit 1' TERM INT 

#TODO allow passing the dom configuration as an option to the command"

create_minios() {

    domfile=$(mktemp "domXXXX") 
    cat<<EOF>$domfile
kernel = "./scratch/mini-os.gz"
memory = "32"
name = "$1"
EOF

    xl create $domfile
    rm $domfile
}

declare -a domains
INDEX=$2

while [ 1 ] ; do

until (( "${#domains[@]}" == "$INDEX" )); do
    domains=( "${domains[@]}"\
      "$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)")
done

    for el in ${domains[@]} ; do
	create_minios $el
    done
    sleep $1
    
    destroy_minios
done
