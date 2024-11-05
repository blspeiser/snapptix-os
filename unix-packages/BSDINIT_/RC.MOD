#!/bin/sh
#
# /etc/rc.modules: module initialization script
#

/sbin/depmod -a

# Load the nvidia module, don't worry, it won't load
# if you don't have an nvidia card
/sbin/modprobe nvidia

# End of file
