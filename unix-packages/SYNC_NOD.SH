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

# Sync up the root file system
run_command "/usr/bin/rsync -avz $ALPHAIP::root /mnt"
if [ $? != "0" ] ; then
	exit 1
fi

# Sync up the user data
# Get things ready to run drbd
HOSTNAME=`cat /mnt/etc/snapptix.conf | grep "^2" | cut -d \  -f 2`
run_command "/bin/hostname $HOSTNAME"
/bin/umount /mnt/home
run_command "rm -f /tmp/drbd.*"
run_command "ln -sf /mnt/var/lib /tmp/lib"
# Now run it in the background
(chroot /mnt /etc/rc.d/drbd start; if [ $? != "0" ] ; then touch /tmp/drbd.failed; else touch /tmp/drbd.success; fi) &
# Printout percentages while it's updating
while [ "1" ] ; do
	PERCENTAGE=`cat /proc/drbd | grep "sync'ed:" | sed "s:^.*sync'ed\:[ ]*::" | sed "s:%.*$::"`
	if [ -n "$PERCENTAGE" ] ; then
		echo "drbd percentage: $PERCENTAGE"
	fi
	if [ -f /tmp/drbd.failed ] || [ -f /tmp/drbd.success ] ; then
		break;
	fi
	sleep 1
done
# Cleanup
run_command "chroot /mnt /etc/rc.d/drbd stop"
run_command "/sbin/ifconfig $1 down"
if [ -f /tmp/drbd.failed ] ; then
	exit 1
fi
exit 0
