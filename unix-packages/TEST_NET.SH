#!/bin/sh
# This program makes sure a set of parameters for the network configs
# works.

. /snapptix/snapptix_functions.sh

PING_COMMAND="ping -c 3 -w 5"

get_arg ()
{
	if [ "`echo $2 | grep $1`" ] ; then
		ARG=`echo "$@" | sed "s:.*$1=::" | cut -d \  -f 1`
	else
		ARG=""
	fi
	echo "Recieved argument \"$ARG\" for $1"
}

# pack our array into a string
PARAMETERS=`echo "${@}"`

# Get the arguments
get_arg "IP" "${PARAMETERS}"
IP=$ARG
get_arg "DEVICE" "${PARAMETERS}"
DEVICE=$ARG
get_arg "PRIMARY_DNS" "${PARAMETERS}"
PRIMARY_DNS=$ARG
if [ ! -n "$IP" ] || [ ! -n "$DEVICE" ] || [ ! -n "$PRIMARY_DNS" ] ; then
	echo "Usage is $0 IP=[ip_address] DEVICE=[device] PRIMARY_DNS=[ip_address] <SECONDARY_DNS=[ip_address]> <DOMAIN=[domain_name]> <BROADCAST=[ip_address]> <GATEWAY=[ip_address]>"
	exit 1
fi

get_arg "SECONDARY_DNS" "${PARAMETERS}"
SECONDARY_DNS=$ARG
get_arg "DOMAIN" "${PARAMETERS}"
DOMAIN=$ARG
get_arg "NETMASK" "${PARAMETERS}"
NETMASK=$ARG
get_arg "BROADCAST" "${PARAMETERS}"
BROADCAST=$ARG
get_arg "GATEWAY" "${PARAMETERS}"
GATEWAY=$ARG

# Just in case
run_command "ifconfig $DEVICE down"

# Bring the device up
IFCONFIG_COMMAND="ifconfig $DEVICE inet $IP"
if [ -n "$NETMASK" ] ; then
	IFCONFIG_COMMAND="$IFCONFIG_COMMAND netmask $NETMASK"
fi
if [ -n "$BROADCAST" ] ; then
	IFCONFIG_COMMAND="$IFCONFIG_COMMAND broadcast $BROADCAST"
fi
run_command "$IFCONFIG_COMMAND"

# Set up the routing table
if [ -n "$GATEWAY" ] ; then
	run_command "route add default gw $GATEWAY dev $DEVICE"
	if [ $? != "0" ] ; then
		echo "Error executing routing command"
	fi
fi

# Make sure we can ping certain parameters
# Note we don't use run_command for the pings
# as we have to catch failure to shutdown the
# device
$PING_COMMAND $PRIMARY_DNS
if [ $? != "0" ] ; then
	run_command "ifconfig $DEVICE down"
	exit 1
fi
if [ -n "$GATEWAY" ] ; then
	$PING_COMMAND $PRIMARY_DNS
	if [ $? != "0" ] ; then
		run_command "ifconfig $DEVICE down"
		exit 1
	fi
fi

# shut device down again
run_command "ifconfig $DEVICE down"

# Everything is OK
exit 0
