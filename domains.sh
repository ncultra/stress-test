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
        xl destroy $e
    done
}


trap 'destroy_minios; echo "caught a signal, exiting..."; exit 1' TERM INT 

#TODO allow passing the dom configuration as an option to the command"

#some of the production servers run out of temporary file
# handles, even when $TMPFIILE points to available storage.
# It seems that bash HERE documents always use /tmp or a variation of it
create_minios() {

   touch $1
   echo 'kernel = "./scratch/mini-os.gz"' > $1
   echo 'memory = "32"' >> $1
   echo -n 'name = "' >> $1
   echo -n $1 >> $1
   echo '"' >> $1
#   cat $1
   xl create -e $1

    rm $1
}

declare -a domains
INDEX=$2

ulimit -c unlimited
ulimit -u unlimited

# place core 
echo $(pwd)/core_%e.%p | tee /proc/sys/kernel/core_pattern 


while [ 1 ] ; do

until (( "${#domains[@]}" == "$INDEX" )); do
    domains=( "${domains[@]}"\
      "$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -c 8)")
done

    for el in ${domains[@]} ; do
        create_minios $el
    done
    sleep $1
    
    destroy_minios
done
