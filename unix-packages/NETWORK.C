#include "gtk_install.h"
#include "network.h"
#include "configs.h"

gboolean *detect_nic_cards(struct network_info *info)
{
	int pid, status, fd;
	char *currmodule;
	char buff[BUFFSIZE];
	int module_num = 0;
	static gboolean retval = TRUE;

	info->modules[0] = NULL;
	info->modules[1] = NULL;
	info->devices = 0;

	for (currmodule = net_modules;
	     strlen(currmodule);
   	     currmodule += strlen(currmodule) + 1)
	{
		pid = fork();
		if (pid < 0)
			THREAD_EXIT(&retval);
		if (pid) /* Parent */
		{
			waitpid(pid, &status, 0);
			if (!status)
			{
				info->modules[module_num] = malloc(strlen(currmodule));
				strcpy(info->modules[module_num], currmodule);
				if (module_num) /* We only need two */
					break;
				module_num++;
			}
		}
		else /* Child */
		{
			execlp("/sbin/modprobe", "/sbin/modprobe", currmodule, NULL);

			/* Shouldn't get here */
			exit(1);
		}
	}
	fd = open("/proc/net/dev", O_RDONLY);
	if (!fd)
		THREAD_EXIT(&retval);
	while (read_until_token(fd, buff, '\n', BUFFSIZE) == READ_OK)
	{
		if (!strncmp(buff, "  eth", 5))
			info->devices++;
	}
	close(fd);
	retval = FALSE;
	THREAD_EXIT(&retval);
}

gboolean *install_nic_modules(struct network_info *info)
{
	int 	pid;
	int 	stdout_pipe[2];
	int 	i;
	char 	buff[BUFFSIZE];
	char 	fullname[BUFFSIZE];
	char 	chroot_path[BUFFSIZE];
	int 	return_status;
	static gboolean retval = TRUE;

	for (i = 0; i < 2; i++)
	{
		if (!info->modules[i])
			continue;

		strcpy(fullname, info->modules[i]);
		strcat(fullname, ".o");

		if (pipe(stdout_pipe) < 0)
			THREAD_EXIT(&retval);

		pid = fork();
		if (pid < 0)
			THREAD_EXIT(&retval)
		else
		if (pid) /* Parent */
		{
			close(stdout_pipe[1]);

			while ((return_status = read_until_token(stdout_pipe[0], buff, '\n', BUFFSIZE)) == READ_OK)
			{
				/* Take off "/sbin/insmod " 13 chars */
				strcpy(chroot_path, "/mnt");
				strcat(chroot_path, buff + 13);
#ifdef DESTRUCTIVE
				if (copy(buff + 13, chroot_path))
					THREAD_EXIT(&retval);
#endif
			}
			if (return_status != READ_DONE)
				THREAD_EXIT(&retval);
		}
		else /* Child */
		{
			close(1);
			dup(stdout_pipe[1]);
			close(stdout_pipe[0]);
	
			execlp("/sbin/modprobe", "/sbin/modprobe", "-v", "-n", info->modules[i], NULL);

			/* Shouldn't get here */
			exit(1);
		}
	}
	retval = FALSE;
	THREAD_EXIT(&retval);
}

gboolean *find_connected_external(struct find_connected_struct *find_connected)
{
	int i;
	char device[16];
	char arguments[BUFFSIZE];
	static gboolean retval = TRUE;
	int pid, status;

	for (i = 0; i < find_connected->check_devices; i++)
	{
		sprintf(device, "eth%d", i);
		pid = fork();
		if (pid < 0)
			THREAD_EXIT(&retval);
		if (pid) /* Parent */
		{
			waitpid(pid, &status, 0);
			if (!status)
			{
				*(find_connected->detected_device) = i;
				retval = FALSE;
				THREAD_EXIT(&retval);
			}
		}
		else /* Child */
		{
			sprintf(arguments, "IP=%s DEVICE=%s PRIMARY_DNS=%s SECONDARY_DNS=%s NETMASK=%s BROADCAST=%s GATEWAY=%s",
				find_connected->ip, device, find_connected->dns1, find_connected->dns2,
				find_connected->netmask, find_connected->broadcast, find_connected->gateway);
			execlp(NET_TEST_SCRIPT, NET_TEST_SCRIPT, arguments, NULL);

			/* Shouldn't get here */
			exit(1);
		}
	}
	THREAD_EXIT(&retval);
}

gboolean *sync_with_alpha(struct sync_with_alpha_struct *sync_struct)
{
	int pid, status, i;
	static gboolean retval = TRUE;
	char device[16];
	char buff[BUFFSIZE];
	FILE *fp;
	int stdout_pipe[2];
	int return_state;
	gfloat percentage;

	update_thread_info(0.0, "Syncing root filesystem");

#ifdef DESTRUCTIVE
	for (i = 0; i < 2; i++)
	{
		if (pipe(stdout_pipe))
			THREAD_EXIT(&retval);
		sprintf(device, "eth%d", i);
		pid = fork();
		if (pid < 0)
			THREAD_EXIT(&retval);
		if (pid) /* Parent */
		{
			close(stdout_pipe[1]);

			/* Read the percentage from the pipe */
			do
			{
				return_state = waitpid(pid, &status, WNOHANG);
				if (read_until_token(stdout_pipe[0], buff, '\n', BUFFSIZE) == READ_ERR)
				{
					return_state = -1;
					break;
				}
				if (sscanf(buff, "drbd percentage: %f", &percentage) == 1)
				{
					/* if the progress_bar is in activity mode unset it */
					pthread_mutex_lock(&mut);
					if (sync_struct->progress_bar->activity_mode)
						gtk_progress_set_activity_mode(sync_struct->progress_bar, FALSE);
					pthread_mutex_unlock(&mut);

					/* set the percentage */
					percentage = percentage / 100.0;
					update_thread_info(percentage, "Syncing network block device");
				}
			}
			while(!return_state);
			close(stdout_pipe[0]);

			if ((!status) && (return_state > 0))
			{
				/* Get the network information from the partial snapptix.conf file */
				fp = fopen("/mnt/etc/snapptix.conf", "r");
				if (!fp)
					THREAD_EXIT(&retval);
				if (fscanf(fp, "1 %s %s %s\n", sync_struct->network_data->primary_hostname,
				    sync_struct->network_data->primary_ip,
				    sync_struct->network_data->primary_public_device) != 3)
					THREAD_EXIT(&retval);
				if (fscanf(fp, "2 %s ___.___.___.___ ___.___.___.___\n",
				    sync_struct->network_data->secondary_hostname) != 1)
					THREAD_EXIT(&retval);
				if (fscanf(fp, "P %s %s\n", buff, sync_struct->network_data->cluster_ip) != 2)
					THREAD_EXIT(&retval);
				if (fscanf(fp, "S %s ___.___.___.___\n", sync_struct->network_data->primary_subnet) != 1)
					THREAD_EXIT(&retval);
				if (!strcmp(sync_struct->network_data->primary_subnet, SNAPPTIX_FILL))
					sync_struct->network_data->primary_subnet[0] = 0x00;
				if (fscanf(fp, "G %s ___.___.___.___\n", sync_struct->network_data->primary_gateway) != 1)
					THREAD_EXIT(&retval);
				if (!strcmp(sync_struct->network_data->primary_gateway, SNAPPTIX_FILL))
					sync_struct->network_data->primary_gateway[0] = 0x00;
				if (fscanf(fp, "DNS1 %s ___.___.___.___\n", sync_struct->network_data->primary_dns1) != 1)
					THREAD_EXIT(&retval);
				if (!strcmp(sync_struct->network_data->primary_dns1, SNAPPTIX_FILL))
					sync_struct->network_data->primary_dns1[0] = 0x00;
				if (fscanf(fp, "DNS2 %s ___.___.___.___\n", sync_struct->network_data->primary_dns2) != 1)
					THREAD_EXIT(&retval);
				if (!strcmp(sync_struct->network_data->primary_dns2, SNAPPTIX_FILL))
					sync_struct->network_data->primary_dns2[0] = 0x00;
				fclose(fp);
				retval = FALSE;
				THREAD_EXIT(&retval);
			}
		}
		else /* Child */
		{
			/* Close output */
			close(1);

			/* Redirect stdout to our pipe */
			dup(stdout_pipe[1]);
			
			/* Close the other end of the pipe */
			close(stdout_pipe[0]);

			execlp("/snapptix/sync_node.sh", "/snapptix/sync_node.sh", device, NULL); 
			exit(1);
		}
	}
#else
	retval = FALSE;
#endif
	THREAD_EXIT(&retval);
}

gboolean *final_sync(struct network_info *network_data)
{
	static gboolean retval = TRUE;
	int status, pid;

#ifdef DESTRUCTIVE
	pid = fork();
	if (pid < 0)
		THREAD_EXIT(&retval);
	if (pid) /* Parent */
	{
		waitpid(pid, &status, 0);
		if (!status)
			retval = FALSE;
	}
	else /* Child */
	{
		execlp("/snapptix/sync_other_node.sh", "/snapptix/sync_other_node.sh", network_data->private_device, NULL);
		exit(1);
	}
#else
	retval = FALSE;
#endif
	THREAD_EXIT(&retval);
}
