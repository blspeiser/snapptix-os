#!/bin/sh

. /snapptix/snapptix_functions.sh

ALPHAIP="192.168.217.1"
BETAIP="192.168.217.2"

if [ -z "$1" ] ; then
	echo "Usage is $0 <device>"
	exit 1;
fi

echo "$0 started parameter $1"

run_command "/sbin/ifconfig $1 down"
run_command "/sbin/ifconfig $1 inet $BETAIP"
# NOTE: We can't use run_command to do the ping check
# as we have to bring the device back down if it fails
/bin/ping -c 3 -w 5 $ALPHAIP
if [ $? != "0" ] ; then
	run_command "/sbin/ifconfig $1 down"
	exit 1
fi
# Here we sync up the alpha with the beta
run_command "/usr/bin/rsync --exclude-from /mnt/etc/rsyncd.exclude -avz /mnt/ $ALPHAIP::root"
run_command "/sbin/ifconfig $1 down"
exit 0
