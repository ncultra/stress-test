
#!/usr/bin/env bash

kill_children() {
    ps --no-headers -C ncat | awk '{print $1}' | xargs kill &> /dev/null
    ps --no-headers -C dd | awk '{print $1}' | xargs kill &> /dev/null
    }


# if $CHILDREN == 0, start a server
# if $CHILDREN == -1, kill all the tests
CHILDREN=$1
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
