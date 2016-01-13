#!/usr/bin/env bash


# $1 == how many seconds to sleep before destroying domains
# $2 == number of domains to start/stop


# bash generate random 32 character alphanumeric string (lowercase only)


if (( $EUID )) ; then
echo "you need root privileges"
    exit 1
fi



#TODO pass the config in as a option
create_minios() {

    domfile=$(mktemp "domXXXX") 
    cat<<EOF>$domfile
kernel = "mini-os.gz"
memory = "32"
name = "$1"
EOF

    xl create $domfile
    cat $domfile
    rm $domfile
}

declare -a domains
INDEX=$2

while [ 1 ] ; do

# create an array of $INDEX random elements
until (( "${#domains[@]}" == "$INDEX" )); do
    domains=( "${domains[@]}"\
      "$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)")
done

echo "${#domains[@]}"
    for el in ${domains[@]} ; do
	create_minios $el
    done
    sleep $1
    
    for el in ${domains[@]} ; do
	xl destroy $el
    done
   
done
