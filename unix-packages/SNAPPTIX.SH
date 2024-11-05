# These are some common functions for our scripts

run_command ()
{
	echo "Executing command $1"
	$1
	if [ $? != "0" ] ; then
		echo "Error executing command $1"
		exit 1
	fi
}
