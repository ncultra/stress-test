#! /usr/bin/bash

for file in fio-tests/*
do
    echo "Running $file in the background"
    fio "$file" &>/dev/null
done &