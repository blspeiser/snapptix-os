/* This file contains templates for config files */

#define LILO_FILE "\
# Created by SNAPPTIX install\n\
\n\
lba32\n\
install=text\n\
boot=%s\n\
image=/vmlinuz\n\
        label=SNAPPTIX\n\
        root=%s\n\
	append=\"video=vga256:320x240 quiet console=/dev/tty2\"\
        read-only\n\
# End of file\n"

/* Remember, home gets mounted AFTER the network block device
 * is created */
#define FSTAB "\
# Created by SNAPPTIX install\n\
\n\
%s\t/ ext3\tdefaults\t0\t0\n\
%s\tswap swap\tdefaults\t0\t\n\
/dev/nbd/0\t/home reiserfs\tnoauto\t0\t0\n\
none\t/proc proc\tdefaults\t0\t0\n"

#define RCCONF "\
# Created by SNAPPTIX install\n\
\n\
TIMEZONE=%s\n\
HOSTNAME=%s\n\
SERVICES=(net rsyncd drbd heartbeat)\n\
\n"

#define RCNET "\n\
#!/bin/sh\n\
# Created by SNAPPTIX install\n\
\n\
if [ \"$1\" = \"start\" ]; then\n\
    /sbin/ifconfig lo 127.0.0.1\n\
    /sbin/modprobe %s\n\
    /sbin/modrpobe %s\n\
    /sbin/ifconfig %s inet %s netmask %s broadcast %s\n\
elif [ \"$1\" = \"stop\" ]; then\n\
    /sbin/ifconfig lo down\n\
    /sbin/ifconfig eth0 down n\
else\n\
    echo \"usage: $0 start|stop\"\n\
fi\n\
\n"

#define DRBDCONF "\n\
resource drbd0 {\n\
	protocol=B\n\
	fsckcmd=echo\n\
\n\
	net {\n\
		sync-max=6M\n\
		connect-int=3\n\
		ping-int=3\n\
		timeout=20\n\
	}\n\
\n\
	on %s {\n\
		device=/dev/nbd/0\n\
		disk=%s\n\
		address=%s\n\
		port=7789\n\
	}\n\
\n\
	on %s {\n\
		device=/dev/nbd/0\n\
		disk=%s\n\
		address=%s\n\
		port=7789\n\
	}\n\
}\n\
\n"

#define HOSTS "\n\
# Created by SNAPPTIX install\n\
\n\
127.0.0.1 localhost\n\
%s %s\n\
%s %s\n\
\n"

#define HACF "\n\
# Creatd by SNAPPTIX install\n\
\n\
bcast %s\n\
keepalive 1\n\
warntime 5\n\
deadtime 10\n\
initdead 30\n\
node %s\n\
node %s\n\
\n"

#define HARESOURCES "\n\
# Created by SNAPPTIX install\n\
\n\
%s %s datadisk::drbd0\n\
\n"

#define RSYNCDCONF "\n\
# Created by SNAPPTIX install\n\
\n\
[root]\n\
	path = /\n\
	uid = root\n\
	gid = root\n\
	read only = no\n\
	list = yes\n\
	hosts allow = %s %s\n\
	exclude from = /etc/rsyncd.exclude\n\
\n"

#define RESOLVCONF "\n\
# Created by SNAPPTIX install\n\
%s\n\
%s\n\
\n"

#define SNAPPTIX_FILL "___.___.___.___"

#define SNAPPTIXCONF "1 %s %s %s\n\
2 %s %s %s\n\
P %s %s\n\
S %s %s\n\
G %s %s\n\
DNS1 %s %s\n\
DNS2 %s %s\n"

