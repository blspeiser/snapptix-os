--- drbd.orig	2003-04-23 15:28:23.000000000 -0500
+++ drbd	2003-04-23 15:32:08.000000000 -0500
@@ -140,21 +140,6 @@
 
 	: ${VERBOSITY:=-1}
 
-	# QQQ thought this could be useful
-	# TERMINAL=/dev/tty
-	# maybe we can use $CONSOLE, which should be exported by init anyways?
-	#
-	for TERMINAL in /proc/self/fd/0 /dev/tty /dev/console ""
-	do
-	   [ -z "$TERMINAL" ] && break
-	   ( [ -t 1 ] < $TERMINAL &> $TERMINAL ) 2>/dev/null && break
-	done
-	if [ -z "$TERMINAL" ] ; then
-		echo "$link: FATAL: no terminal"
-		exit -1
-	fi
-
-
 # frame work for commandline options
 	# I use getopt, if it is in PATH. if getopt is not found, it will
 	# (hopefully) silently work anyways, though the user has to supply correct
@@ -1802,9 +1787,10 @@
 			(
 				trap "exit 1" INT
 				trap "echo 'All resources connected.'; exit 0" TERM
-				sleep 3         # show the prompt with a little delay.
-				ask_for_abort && kill -USR1 $$
-			)  < $TERMINAL &> $TERMINAL &
+				sleep 7         # just say we're the primary if we dont
+								# connect for a while
+				kill -USR1 $$
+			) &
 			debug 1 "[$!] ask_for_abort"
 
 			# on INT, propagate; this is <CTRL-C> by user or generated in
