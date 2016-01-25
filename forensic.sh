#!/usr/bin/bash

uname -a

cat /etc/redhat-release 

cat /var/log/xen-dmsg

xl info
xl list
lsmod

dmesg

# SAR file can become larger than the expected size of a normal log file.
# best if sar output is redirected away from partitions that are low on capacity.
sar -o SAR$PPID -i 10 > /dev/null 2>&1 &

ps aux 

rpm -qa

df
mount

w

top -b -n 1 -c
