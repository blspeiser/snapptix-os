#include "gtk_install.h"
#include "configs.h"

gboolean copy(char *from, char *to)
{
	int fd1, fd2;
	int bytes_read;
	char buff[BUFFSIZE];

	fd1 = open(from, O_RDONLY);
	if (fd1 < 0)
		return TRUE;
	fd2 = open(to, O_WRONLY | O_CREAT, 0644);
	if (fd2 < 0)
		return TRUE;
	while ((bytes_read = read(fd1, buff, BUFFSIZE)) > 0)
		write(fd2, buff, bytes_read);
	close(fd1);
	close(fd2);
	return FALSE;
}

gboolean *setup_configs(struct setup_configs_struct *setup_info)
{
	FILE		*fp;
	static gboolean retval = TRUE;
	char		salt[12];
	int		i;
	char 		filename[BUFFSIZE];
	char 		short_filename[BUFFSIZE];

#ifdef DESTRUCTIVE
	/* LILO */
	debug_output("Writing lilo.conf\n");
	fp = fopen("/mnt/etc/lilo.conf", "w");
	if (!fp)
		THREAD_EXIT(&retval);
	fprintf(fp, LILO_FILE, setup_info->mount_data->root_device,
			       setup_info->mount_data->snapptix);
	fclose(fp);

	/* FSTAB */
	debug_output("Writing fstab\n");
	fp = fopen("/mnt/etc/fstab", "w");
	if (!fp)
		THREAD_EXIT(&retval);
	fprintf(fp, FSTAB, setup_info->mount_data->snapptix,
		setup_info->mount_data->swap);
	fclose(fp);

	/* XF86Config */
	debug_output("Writing XF86Config\n");
	if (copy("/tmp/XF86Config.new", "/mnt/etc/X11/XF86Config"))
		THREAD_EXIT(&retval);

	/* We only need a new password file if this is an alpha setup */
	if (!setup_info->node_type)
	{
		/* Shadow */
		debug_output("Writting shadow\n");
        	/* initialize the random function */
        	srandom(time(NULL));

        	/* crypt requires an up to 8 character salt that *
	 	 * has characters in the set [a-zA-Z0-9./] *
	 	 * since we're using MD5 passwords we prefix our *
	 	 * salt with $1$ and end it with $ */
        	salt[0] = '$';
        	salt[1] = '1';
        	salt[2] = '$';
        	for (i = 3; i < 11; i++)
        	{
			do
			{
				salt[i] = (random() % 64) + '.';
                	} while (((salt[i] > '9') && (salt[i] < 'A')) ||
				((salt[i] > 'Z') && (salt[i] < 'a')));
		}
        	salt[11] = '$';
		salt[12] = 0x00;

        	/* Shadow format <user>:<encrypted pass>:
         	 * <time since epoch when changed (in days)>:<unused fields> */
		fp = fopen("/mnt/etc/shadow", "w");
		if (!fp)
			THREAD_EXIT(&retval);
		fprintf(fp, "root:%s:%ld::::::\n", crypt(setup_info->password, salt),
		                           	   (unsigned long)(time(NULL)/(60*60*24)));
		fclose(fp);
	}

	/* rc.conf */
	/* NOTE: We have a copy of these for each node */
	debug_output("Writing rc.conf\n");
	sprintf(short_filename, "rc.conf.%d", setup_info->node_type + 1);
	strcpy(filename, "/mnt/etc/");
	strcat(filename, short_filename);
	fp = fopen(filename, "w");
	if (!fp)
		THREAD_EXIT(&retval);
	fprintf(fp, RCCONF, setup_info->timezone,
		setup_info->node_type ? setup_info->network_data->secondary_hostname
				      : setup_info->network_data->primary_hostname);
	fclose(fp);
	if (symlink(short_filename, "/mnt/etc/rc.conf"))
		THREAD_EXIT(&retval);

	/* /etc/rc.d/net */
	/* NOTE: We have a copy of these for each node */
	debug_output("Writing rc.d/net\n");
	sprintf(short_filename, "net.%d", setup_info->node_type + 1);
	strcpy(filename, "/mnt/etc/rc.d/");
	strcat(filename, short_filename);
	fp = fopen(filename, "w");
	if (!fp)
		THREAD_EXIT(&retval);
	fprintf(fp, "# Created by Snapptix Installer\n");
	fprintf(fp, "\n");
	fprintf(fp, "if [ \"$1\" = \"start\" ]; then\n");
	fprintf(fp, "\t/sbin/ifconfig lo 127.0.0.1\n");
	for (i = 0; i < 2; i++)
	{
		if (setup_info->network_data->modules[i])
			fprintf(fp, "\t/sbin/modprobe %s\n", setup_info->network_data->modules[i]);
	}
	fprintf(fp, "\tifconfig %s inet %s ", setup_info->network_data->public_device,
		setup_info->network_data->ip);
	if (strcmp(setup_info->network_data->subnet, SNAPPTIX_FILL))
		fprintf(fp, "netmask %s ", setup_info->network_data->subnet);
	fprintf(fp, "\n");
	if (strlen(setup_info->network_data->gateway))
		fprintf(fp, "\troute add default gw %s dev %s\n", setup_info->network_data->gateway,
			setup_info->network_data->public_device);
	fprintf(fp, "\tifconfig %s inet %s netmask %s broadcast %s\n",
		setup_info->network_data->private_device,
		setup_info->node_type ? LOCAL_NET_BETA_IP : LOCAL_NET_ALPHA_IP,
		LOCAL_NET_SUBNET, LOCAL_NET_BROADCAST);
	fprintf(fp, "elif [ \"$1\" = \"stop\" ]; then\n");
	fprintf(fp, "\t/sbin/ifconfig lo down\n");
	fprintf(fp, "\t/sbin/ifconfig eth0 down\n");
	fprintf(fp, "\t/sbin/ifconfig eth1 down\n");
	fprintf(fp, "else\n");
	fprintf(fp, "\techo \"usage: $0 start|stop\"\n");
	fprintf(fp, "fi\n");
	fclose(fp);
	unlink("/mnt/etc/rc.d/net");
	/* make it an executable */
	if (symlink(short_filename, "/mnt/etc/rc.d/net"))
		THREAD_EXIT(&retval);
	if (chmod("/mnt/etc/rc.d/net", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH))
		THREAD_EXIT(&retval);

	/* /etc/drbd.conf */
	if (!setup_info->node_type)
	{
		debug_output("Writing drbd.conf\n");
		fp = fopen("/mnt/etc/drbd.conf", "w");
		if (!fp)
			THREAD_EXIT(&retval);
		fprintf(fp, DRBDCONF, setup_info->network_data->primary_hostname,
			setup_info->mount_data->user,
			LOCAL_NET_ALPHA_IP,
			setup_info->network_data->secondary_hostname,
			setup_info->mount_data->user,
			LOCAL_NET_BETA_IP);
		fclose(fp);
	}

	/* /etc/hosts */
	if (!setup_info->node_type)
	{
		debug_output("Writing hosts\n");
		fp = fopen("/mnt/etc/hosts", "w");
		if (!fp)
			THREAD_EXIT(&retval);
		fprintf(fp, HOSTS, LOCAL_NET_ALPHA_IP, setup_info->network_data->primary_hostname,
			LOCAL_NET_BETA_IP, setup_info->network_data->secondary_hostname);
		fclose(fp);
	}

	/* /etc/ha.d/ha.cf */
	/* NOTE: We have a copy of these for each node */
	debug_output("Writing ha.d/ha.cf\n");
	sprintf(short_filename, "ha.cf.%d", setup_info->node_type + 1);
	strcpy(filename, "/mnt/etc/ha.d/");
	strcat(filename, short_filename);
	fp = fopen(filename, "w");
	if (!fp)
		THREAD_EXIT(&retval);
	fprintf(fp, HACF, setup_info->network_data->private_device,
		setup_info->network_data->primary_hostname,
		setup_info->network_data->secondary_hostname);
	fclose(fp);
	if (symlink(short_filename, "/mnt/etc/ha.d/ha.cf"))
		THREAD_EXIT(&retval);

	/* /etc/ha.d/haresources */
	if (!setup_info->node_type)
	{
		debug_output("Writing haresources\n");
		fp = fopen("/mnt/etc/ha.d/haresources", "w");
		fprintf(fp, HARESOURCES, setup_info->network_data->primary_hostname,
			setup_info->network_data->cluster_ip);
		fclose(fp);
	}

	/* /etc/rsyncd.conf */
	if (!setup_info->node_type)
	{
		debug_output("Writing rsyncd.conf\n");
		fp = fopen("/mnt/etc/rsyncd.conf", "w");
		if (!fp)
			THREAD_EXIT(&retval);
		fprintf(fp, RSYNCDCONF, LOCAL_NET_ALPHA_IP, LOCAL_NET_BETA_IP);
		fclose(fp);
	}

	/* /etc/snapptix.conf */
	fp = fopen("/mnt/etc/snapptix.conf", "w");
	if (!fp)
		THREAD_EXIT(&retval);
	if (setup_info->node_type)
	{
		debug_output("Writing full snapptix.conf\n");
		fprintf(fp, SNAPPTIXCONF, setup_info->network_data->primary_hostname,
			setup_info->network_data->primary_ip, setup_info->network_data->primary_public_device, 
			setup_info->network_data->secondary_hostname, setup_info->network_data->ip,
			setup_info->network_data->public_device, SNAPPTIX_FILL,
			setup_info->network_data->cluster_ip, setup_info->network_data->primary_subnet,
			setup_info->network_data->subnet, setup_info->network_data->primary_gateway,
			setup_info->network_data->gateway, setup_info->network_data->primary_dns1,
			setup_info->network_data->dns1, setup_info->network_data->primary_dns2,
			setup_info->network_data->dns2);
	}
	else
	{
		debug_output("Writing partial snapptix.conf\n");
		fprintf(fp, SNAPPTIXCONF, setup_info->network_data->primary_hostname,
			setup_info->network_data->ip, setup_info->network_data->public_device,
			setup_info->network_data->secondary_hostname, SNAPPTIX_FILL,
			SNAPPTIX_FILL, SNAPPTIX_FILL, setup_info->network_data->cluster_ip,
			setup_info->network_data->subnet, SNAPPTIX_FILL,
			setup_info->network_data->gateway, SNAPPTIX_FILL,
			setup_info->network_data->dns1, SNAPPTIX_FILL,
			setup_info->network_data->dns2, SNAPPTIX_FILL);
	}
	fclose(fp);

	/* /etc/resolv.conf */
	/* NOTE: We have a copy of these for each node */
	debug_output("Writing /etc/resolv.conf\n");
	sprintf(short_filename, "resolv.conf.%d", setup_info->node_type + 1);
	strcpy(filename, "/mnt/etc/");
	strcat(filename, short_filename);
	fp = fopen(filename, "w");
	if (!fp)
		THREAD_EXIT(&retval);
	fprintf(fp, "# Created by Snapptix Install\n");
	fprintf(fp, setup_info->network_data->dns1);
	fprintf(fp, "\n");
	if (strcmp(setup_info->network_data->dns2, SNAPPTIX_FILL))
	{
		fprintf(fp, setup_info->network_data->dns2);
		fprintf(fp, "\n");
	}
	fclose(fp);
	unlink("/mnt/etc/resolv.conf");
	if (symlink(short_filename, "/mnt/etc/resolv.conf"))
		THREAD_EXIT(&retval);
#endif

	retval = FALSE;
	THREAD_EXIT(&retval);
}

gboolean *setup_kernel()
{
	static gboolean retval = TRUE;
	int pid;
	int status;

#ifdef DESTRUCTIVE
	/* Execute LILO */
	pid = fork();
	if (pid < 0)
		THREAD_EXIT(&retval);
	if (pid) /* Parent */
	{
		waitpid(pid, &status, 0);
		if (status)
			THREAD_EXIT(&retval);
	}
	else /* Child */
	{
		/* Chroot to our mounted directory */
		if (chroot("/mnt"))
			THREAD_EXIT(&retval);

		execl("/sbin/lilo", "/sbin/lilo", NULL);
		exit(1); /* Shouldn't get here */
	}
#endif

	retval = FALSE;
	THREAD_EXIT(&retval);
}
