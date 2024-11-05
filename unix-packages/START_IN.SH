#!/bin/bash

TIMEOUT="30"
RESOLUTION="800x600"
VERTREFRESH="40-72"
HORIZSYNC="30-50"

PATH="/bin:/sbin:/usr/bin:/usr/sbin:/usr/X11R6/bin:/snapptix"
PID=""

# Outputs things to the debug log
function debug_output()
{
	echo $1 >> /tmp/start_install.log
	
	return
}

# Outputs things to the consoles for the user and the debug log
function user_output()
{
	echo $1 >> /dev/vc/1
	echo $1 >> /tmp/start_install.log

	return
}

# Get the PID of a running process
# return it with the variable PID
function pid_of()
{
	local TMPPID

	debug_output "Finding PID of $1"
	TMPPID=`ps ax | grep $1 | grep -v grep`
	if [ "$TMPPID" ] ; then
		PID=`echo $TMPPID | awk '{ print $1 }'`
		debug_output "Got PID $PID"
	else
		debug_output "Didn't find PID for $1"
		PID=""
	fi

	return
}

# Outputs images to the framebuffer
# We only have to run pngtofb once, from then on we just change
# the link it's looking at and send it a USR2 signal.
function framebuffer_output()
{
	local CURRTIME

	# Update our link
	debug_output "Linking /tmp/currimage.png to $1"
	ln -sf $1 /tmp/currimage.png
	pid_of "pngtofb"
	if [ ! "$PID" ] ; then
		debug_output "Starting pngtofb"
		pngtofb /tmp/currimage.png &
	else
		# Send the running pngtofb a USR2 to reload
		debug_output "Sending pngtofb PID - $PID a USR2 signal"
		kill -USR2 $PID
	fi

	return
}

# Load the nvidia module, if it fails it just means they don't have an nvidia card,
# so we don't really care about the output.
insmod /lib/modules/2.4.20/kernel/drivers/video/nvidia.o &> /dev/null

# Make things prettier
# Disable local echo and "canonical" mode
stty -icanon -echo

# Start with a splash screen saying were configuring X
framebuffer_output "/snapptix/images/x_config_splash.png"

# This way X puts the config file in the tmp directory, /root doesn't exist on the
# CD
HOME=/tmp

# Run XFree86 in autoconfigure mode, but after 30 seconds kill it
# It unfortunately has a tendancy to hang on some machines
debug_output "Starting XFree86 -configure"
(XFree86 -configure -allowMouseOpenFail &) &> /tmp/X_Output.log
TIME=0
# PID just has to be "something" to start the loop.  Bash doesn't have do-whiles
PID=defined
while [ "$PID" ] ; do
	debug_output "XFree86 -configure still running time is: $TIME"
	if (("$TIME" >= "$TIMEOUT")) ; then
		debug_output "XFree86 -configure took too long, killing it"
		# Try giving it a Ctrl-C first
		kill -INT $PID
		sleep 2;
		# If this doesn't work kill it hard
		pid_of "XFree86"
		if [ "$PID" ] ; then
			kill -9 $PID
		fi
		break
	fi
	# Give XFree86 a grace period to start up, if it needs it
	pid_of "XFree86"
	if (("$TIME" <= 4)) && [ ! "$PID" ] ; then
		debug_output "No PID for XFree86, but still in grace period time is: $TIME"
		PID=defined
	fi
	sleep 1;
	TIME=$(($TIME+1))
done

# NOTE: even if we had to kill XFree86 -config we can still usually use the config file
# it makes
if [ ! -f "/tmp/XF86Config.new" ] ; then
	framebuffer_output "/snapptix/images/x_config_failed_splash.png"
	debug_output "XFree86 -configure didn't make a XF86Config.new file"
	exit 1
fi

# Get the depth right.  If we can, we want to use 16bpp, but we'll settle for 8bpp
# if need be.

# This generates a newline seperated list of depths
DEPTH_LIST=`cat /tmp/XF86Config.new | grep Depth | sed "s/[ \t]*Depth[ \t]*//"`

if [ "echo $DEPTH_LIST | grep 16" ] ; then
	debug_output "Using depth of 16"
	USE_DEPTH="16"
elif [ "echo $DEPTH_LIST | grep 8" ] ; then
	debug_output "Using depth of 8"
	USE_DEPTH="8"
fi
sed -i "/Monitor[ \t]*\"Monitor0\"/a	DefaultDepth $USE_DEPTH" /tmp/XF86Config.new

# If we are using the nvidia driver turn off the splash logo
if [ "grep \"^[ \t]*Driver[ \t]*\"nvidia\"\"" ] ; then
	debug_output "Using NVidia Driver"
	sed -i "/^[ \t]*BusID.*$/a	Option \"NoLogo\" \"on\"" /tmp/XF86Config.new
fi

# Set the resolution
sed -i "/[ \t][ \t]*Depth[ \t]*$USE_DEPTH/a		Modes      \"$RESOLUTION\"" /tmp/XF86Config.new
debug_output "Using resolution of $RESOLUTION"

# Set the HorizSync to something reasonable
sed -i "/Section \"Monitor\"/a	HorizSync $HORIZSYNC" /tmp/XF86Config.new

# Set the VertRefresh to something reasonable
sed -i "/Section \"Monitor\"/a	VertRefresh $VERTREFRESH" /tmp/XF86Config.new

# Need to tell user to move mouse here (splash_screen, only for serial though)
framebuffer_output "/snapptix/images/mouse_detect_splash.png"

# Might also want to tell them to switch PC to MS (if they have a switch, serial only)
debug_output "Running mdetect"
MOUSE_DETECT=`mdetect -x`
if [ ! "$MOUSE_DETECT" ] ; then
	framebuffer_output "/snapptix/images/mouse_detect_failed_splash.png"
	debug_output "mdetect failed to print anything"
	exit 1
fi
MOUSE_DEVICE=`echo $MOUSE_DETECT | cut -d \  -f 1`
debug_output "Using mouse device $MOUSE_DEVICE"
MOUSE_PROTOCOL=`echo $MOUSE_DETECT | cut -d \  -f 2`
debug_output "Using mouse protocol $MOUSE_PROTOCOL"
sed -i "s:/dev/mouse:$MOUSE_DEVICE:" /tmp/XF86Config.new
sed -i "s:auto:$MOUSE_PROTOCOL:" /tmp/XF86Config.new

debug_output "Running gtk_install"
xinit /snapptix/gtk_install -- vt7 -xf86config /tmp/XF86Config.new &> /tmp/X_Output.log

# Hopefully no user get's here
framebuffer_output "/snapptix/images/x_init_failed_splash.png"
exit 1
