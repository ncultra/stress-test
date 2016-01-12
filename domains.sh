#!/usr/bin/env bash


if (( $EUID )) ; then
echo "you need root privileges"
    exit 1
fi

echo "trashing mini-os domain"

while [ 1 ] ; do

    xl create minios/domain_config
    sleep $1
    echo xl destroy mos
done
