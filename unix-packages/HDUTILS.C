#include "gtk_install.h"

/* This file contains a collection of functions for
 * creating/editing/checking partitions/filesystems */

/* This function makes a partition swap space with the mkswap command */
gboolean *make_swap(char *partition)
{
	int 		pid;
	int 		status;
	static gboolean retval = TRUE;

#ifdef DESTRUCTIVE
	pid = fork();
	if (pid < 0)
		THREAD_EXIT(&retval);
	if (pid > 0) /* Parent */
	{
		waitpid(pid, &status, 0);
		if (status)
			THREAD_EXIT(&retval);
		/* Activate the swap partition */
		if (swapon(partition, 0))
			THREAD_EXIT(&retval);
	}
	else /* Child */
	{
		execlp("mkswap", "mkswap", partition, NULL);

		/* Shouldn't get here */
		exit(1);
	}
#endif
	retval = FALSE;
	THREAD_EXIT(&retval);
}

/* This function makes the ext3 snapptix root partition with mke2fs */
gboolean *make_snapptix(char *partition)
{
	int 		pid;
	int 		status;
	static gboolean	retval = TRUE;

#ifdef DESTRUCTIVE
	pid = fork();
	if (pid < 0)
		THREAD_EXIT(&retval);
	if (pid > 0) /* Parent */
	{
		waitpid(pid, &status, 0);
		if (status)
			THREAD_EXIT(&retval);
	}
	else /* Child */
	{
		execlp("mke2fs", "mke2fs", "-j", partition, NULL);
	
		/* Shouldn't get here */
		exit(1);
	}
#endif
	retval = FALSE;
	THREAD_EXIT(&retval);
}

/* uses mkreiserfs to create a user partition */
gboolean *make_user(char *partition)
{
	int 		pid;
	int 		status;
	int 		stdin_pipe[2];
	int 		stderr_pipe[2];
	char 		line[BUFFSIZE];
	float		current_percent;
	static gboolean	retval = TRUE;

#ifdef DESTRUCTIVE
	update_thread_info(0.0, NULL);

	if (pipe(stdin_pipe))
		THREAD_EXIT(&retval);
	if (pipe(stderr_pipe))
		THREAD_EXIT(&retval);
	pid = fork();
	if (pid < 0)
		THREAD_EXIT(&retval);
	if (pid > 0) /* Parent */
	{
		while (1)
		{
			/* make sure the child hasn't exited */
			/* this way we can catch errors and exits */
			if (waitpid(pid, &status, WNOHANG))
				THREAD_EXIT(&retval);

			/* Read the first part of the line */
			if (read(stderr_pipe[0], line, 23) <= 0)
				THREAD_EXIT(&retval);
			line[23] = 0x00;

			/* See if it's the status line */
			if (!strcmp(line, "Initializing journal - "))
				break;

			/* Otherwise read off the rest of the line */
			if (read_until_token(stderr_pipe[0], line, '\n', BUFFSIZE))
				THREAD_EXIT(&retval);
		}
		current_percent = 0.0;
		
		/* Read and update status */
		while (1)
		{
			if (read(stderr_pipe[0], line, 1) != 1)
				THREAD_EXIT(&retval);
			if ((line[0] != '%') && (line[0] != '0')) /* Ignore % and 0 */
			{
				if (line[0] == '\n') /* We're done after a carriage
							return */
					break;
				if (line[0] == '.') /* Each '.' represents 5% */
					current_percent += 0.04;
				else
				{
					/* The last 1 in the output is for 100 percent */
					if ((line[0] == '1') && (current_percent > 0.1))
					{
						current_percent = 1.0;
					}
					else
					{
						line[1] = 0x00;
						current_percent = atof(line) / 10.0;
					}
				}
				update_thread_info(current_percent, NULL);
			}
		}
	}
	else
	{
		/* Setup the pipes */
		close(0);
		dup(stdin_pipe[0]);
		close(2);
		dup(stderr_pipe[1]);
		close(stdin_pipe[1]);
		close(stderr_pipe[0]);

		/* Make the label snapptix and don't ask for confirmation */
		execlp("mkreiserfs", "mkreiserfs", "-ff", "-l", "SNAPPTIX", partition, NULL);

		/* Shouldn't get here */
		exit(1);
	}
#endif

	/* Wait for mkreiserfs to finish (syncing takes a
	 * little while) */
	waitpid(pid, &status, 0);
	if (!status)
		retval = FALSE;
	THREAD_EXIT(&retval);
}


/* While this isn't really a hd utility, it is used to determine the 
 * swap partition size, so I put it in here */
unsigned long get_memsize()
{
	int fd;
	char line[BUFFSIZE];
	char *start, *end;

	fd = open("/proc/meminfo", O_RDONLY);

	/* First line is garbage */
	if (read_until_token(fd, line, '\n', BUFFSIZE))
	{
		close(fd);
		return 0;
	}

	if (read_until_token(fd, line, '\n', BUFFSIZE))
	{
		close(fd);
		return 0;
	}

	/* Start at four read off "Mem:" header */
	start = line + 4;
	while (*start == ' ')
		start++;

	/* Read until whitespace */
	end = start;
	while (*end != ' ')
		end++;
	*end = 0x00;

	return atoi(start);	
}

/* Gets the size of a harddrive */
unsigned long long get_hdsize(char *disc)
{
	int 			pid;
	int 			stdin_pipe[2];
	int 			output_pipe[2];
	char 			line[BUFFSIZE];
	int 			status;
	unsigned long long 	size;

	if (pipe(stdin_pipe))
		return 0;
	if (pipe(output_pipe))
		return 0;
	pid = fork();
	if (pid < 0)
		return 0;
	if (pid)
	{
		/* Close the parts we aren't using */
		close(stdin_pipe[0]);
		close(output_pipe[1]);

		read_until_token(output_pipe[0], line, '\n', BUFFSIZE);
		waitpid(pid, &status, 0);
		if (status)
			return 0;
		else
		{
			/* Return size in bytes */
			size = atoll(line) * 1024;
			return size;
		}
	}
	else
	{
		/* Redirect the output and input */
		close(0);
		dup(stdin_pipe[0]);
		close(1);
		dup(output_pipe[1]);
		close(2);
		dup(output_pipe[1]);

		/* Close everything we don't use */
		close(stdin_pipe[1]);
		close(output_pipe[0]);

		/* Run sfdisk */
		execlp("sfdisk", "sfdisk", "-s", disc, NULL);

		/* We shouldn't get here */
		exit(1);
	}
}

/* makes and executes a shell script to create a new parition table */
gboolean *create_partitions(char *disc)
{
	unsigned long 		swap_size;
	unsigned long long	hd_size;
	int 			pid;
	FILE 			*fp;
	int 			status;
	static 			gboolean retval = TRUE;

	/* Your swap should be about twice your memory */
	/* We do partitions in sectors (512 bytes) */
	swap_size = get_memsize() * 2;
	if (!swap_size)
		THREAD_EXIT(&retval);

	/* Grab the harddrive size */
	hd_size = get_hdsize(disc);
	if (!hd_size)
		THREAD_EXIT(&retval);

	/* Make sure we have enough space */
	if ((swap_size + SNAPPTIX_SIZE) >= hd_size)
		THREAD_EXIT(&retval);

#ifdef DESTRUCTIVE
	/* Create the partition table */
	/* Ok, we are going to use Megabytes to define our partitions
	   and while this may not align our partitions on cylinder
	   boundaries, that isn't a problem for Linux.  We'll also
	   leave the geometry the same, as things typically don't
	   use it. */
	/* Our partition table looks like this
	 * part1	memorysize * 2		SWAP
	 * part2	Distro Size		LINUX
	 * part3	Everything else		LINUX */
	/* NOTE: We create a shell script to execute sfdisk, as sfdisk
	 * likes to have a controlling tty, so we have to execute it with
	 * bash */
	fp = fopen(PARTITION_SH, "w+");
	fprintf(fp, "#!/bin/sh\n\n");
	fprintf(fp, "sfdisk -O %s -uM -L -q %s << EOF\n", PARTITION_BAK, disc);
	fprintf(fp, "0,%ld,%s\n", swap_size / (1024 * 1024), SWAP_ID);
	fprintf(fp, ",%d,%s,*\n", SNAPPTIX_SIZE / (1024 * 1024), LINUX_ID);
	fprintf(fp, ",,%s\n", LINUX_ID);
	fprintf(fp, "EOF");
	fclose(fp);

	if (chmod(PARTITION_SH, S_IXUSR))
		THREAD_EXIT(&retval);

	pid = fork();
	if (pid < 0)
		THREAD_EXIT(&retval);
	if (pid) /* Parent */
	{
		waitpid(pid, &status, 0);
		if (status)
			THREAD_EXIT(&retval);
	}
	else
	{
		/* Startup bash to interpret our script */
		execl(PARTITION_SH, PARTITION_SH, NULL);

		/* We shouldn't get here */
		exit(1);
	}
#endif
	retval = FALSE;
	THREAD_EXIT(&retval);
}
	
/* Forks and runs a badblocks on the device of your choice */
/* Returns TRUE if there was a problem, FALSE otherwise */
/* This gets called by a thread, hence the cray static pointer */
/* return value (pthread_join) */
gboolean *bad_block_scan(char *device_path)
{
	int 	pid;
	int 	stderr_pipe[2];
	int 	curr_blocks = 1;
	int 	total_blocks = 1;
	int 	status;
	unsigned short return_val;
	char 	buff[BUFFSIZE];
	struct stat file_stat;
	/* This is OUR return value */
	static 	gboolean retval = TRUE;

#ifdef DESTRUCTIVE
	if (pipe(stderr_pipe) < 0)
		THREAD_EXIT(&retval);
	pid = fork();
	if (pid < 0) /* if you can't fork in UNIX you've got really big
			problems */
		THREAD_EXIT(&retval)
	else
	if (pid) /* Parent */
	{
		close(stderr_pipe[1]); /* We aren't writting to this
					  pipe */

		/* Read off the intro garbage */
		if (read(stderr_pipe[0], buff, 41) < 0)
			THREAD_EXIT(&retval);

		buff[0] = ' ';

		while (1)
		{
			/* Read and parse the status string */

			/* Read off the starting whitespace */
			if ((return_val = read_off_whitespace(stderr_pipe[0], buff)))
			{
				if (return_val == READ_ERR)
					THREAD_EXIT(&retval)
				else
					break;
			}

			/* Read up until the / */
			/* Remember we already read one digit */
			if ((return_val = read_until_token(stderr_pipe[0], buff + 1, '/', BUFFSIZE)))
			{
				if (return_val == READ_ERR)
					THREAD_EXIT(&retval)
				else
					break;
			}
			curr_blocks = atoi(buff);

			/* Read off the middle white space */
			if ((return_val = read_off_whitespace(stderr_pipe[0], buff)))
			{
				if (return_val == READ_ERR)
					THREAD_EXIT(&retval)
				else
					break;
			}

			/* Now read until the backspace */	
			/* Remember we already read one digit */
			if ((return_val = read_until_token(stderr_pipe[0], buff + 1, 0x08, BUFFSIZE)))
			{
				if (return_val == READ_ERR)
					THREAD_EXIT(&retval)
				else
					break;
			}
			total_blocks = atoi(buff);

			update_thread_info((float)curr_blocks/(float)total_blocks, NULL);
		}
		waitpid(pid, &status, 0);

		/* Make sure the BADBLOCKS_FILE doesn't have anything
		 * in it */
		if (!stat(BADBLOCKS_FILE, &file_stat))
		{
			if (file_stat.st_size == 0)
				retval = FALSE;
		}
		THREAD_EXIT(&retval);
	}
	else /* Child */
	{
		close(2);
		dup(stderr_pipe[1]); /* Remember dup assigns the lowest fd it
					can */

		close(0); /* Just in case */
		close(1);
		close(stderr_pipe[0]); /* We aren't reading from this pipe */

		execlp("badblocks", "badblocks", "-s", device_path, "-o", BADBLOCKS_FILE, NULL);

		exit(1); /* You shouldn't get here */
	}
#endif
	retval = FALSE;
	THREAD_EXIT(&retval);
}

/* This function forks and runs debugreiserfs on a partition */
/* Returns 1 if the partition is SNAPPTIX user data */
/* Returns 0 if the partition isn't */
/* Returns -1 on error */
short check_for_user_data(char *partition)
{
	char 		line[BUFFSIZE];
	int 		output_pipe[2];
	int 		pid;
	short 		returnval;
	int 		status;
	unsigned short 	read_status;

	printf("Checking partition %s for user data\n", partition);

	if (pipe(output_pipe) < 0)
		return -1;
	pid = fork();
	if (pid < 0)
		return -1;
	else
	if (pid) /* Parent */
	{
		close(output_pipe[1]);

		do
		{
			if ((read_status = read_until_token(output_pipe[0], line, 0x0A, BUFFSIZE)))
			{
				if (read_status == READ_ERR)
					returnval = -1;
				else
				if (read_status == READ_DONE)
					returnval = 0;
				break;
			}
			printf("Read %s\n", line);
			if (strstr(line, "no filesystem found"))
			{
				returnval = 0;
				break;
			}
			/* We determine snapptix userdata by reiserfs VOLUME
			 * label */
			if (strstr(line, "LABEL: SNAPPTIX"))
			{
				printf("Found a user partition\n");
				returnval = 1;
				break;
			}
		} while (1);

		waitpid(pid, &status, 0);
		return returnval;
	}
	else
	{
		/* Stdout and Stderr both go to our pipe */
		close(1);
		dup(output_pipe[1]);

		close(2);
		dup(output_pipe[1]);

		close(0); /* Just in case */
		close(output_pipe[0]);

		execlp("debugreiserfs", "debugreiserfs", partition, NULL);

		exit(1); /* Shouldn't get here */
	}
}

/* We find all the ide drives here, even though we only use one.
 * The multiple drive code is for possible future expansion */
/*--RECURSION--*/
struct discs **find_discs(char *currpath, struct discs **node)
{
	DIR *dir;
	struct stat file_stats;
	struct dirent *dir_ent;
	char *new_path;

	if (! (dir = opendir(currpath)))
		return node;
	while ((dir_ent = readdir(dir)) != NULL)
	{
		/* Don't recurse out */
		if ((strcmp(dir_ent->d_name, ".")) && (strcmp(dir_ent->d_name, "..")))
		{
			/* Construct the new path string */
			new_path = (char *)malloc(strlen(currpath) + 1 + strlen(dir_ent->d_name));
			strcpy(new_path, currpath);
			strcat(new_path, "/");
			strcat(new_path, dir_ent->d_name);

			/* Don't follow symlinks */
			lstat(new_path, &file_stats);
			if (S_ISDIR(file_stats.st_mode))
				/*--RECURSION--*/
				node = find_discs(new_path, node);
			else
			if (! strcmp(dir_ent->d_name, "disc"))
			{
				/* Found one, slap it on the good ol' linked list */
				*node = (struct discs *)malloc(sizeof(struct discs));
				(*node)->path = (char *)malloc(strlen(new_path));
				strcpy((*node)->path, new_path);
				(*node)->next = NULL;
				node = &(*node)->next;
			}
			free(new_path);
		}
	}
	closedir(dir);
	return node;
}

/* Searches all partitions of the the given disks looking for a SNAPPTIX
   user data partition */
char *find_snapptix_partition(struct discs *root)
{
	struct discs *node;
	size_t num_of_chars;
	char *dir_path;
	DIR *dir;
	struct dirent *dir_ent;
	short return_value;
	char *partition_path;

	printf("Find snapptix partition\n");

	partition_path = NULL;
	for (node = root; node; node = node->next)
	{
		num_of_chars = strlen(node->path) - 5;
		dir_path = (char *)malloc(num_of_chars);
		strncpy(dir_path, node->path, num_of_chars);
		printf("Opening directory %s\n", dir_path);
		if (! (dir = opendir(dir_path)))
			return NULL;
		while ((dir_ent = readdir(dir)))
		{
			printf("Checking directory entry %s\n", dir_ent->d_name);
			if (!strncmp(dir_ent->d_name, "part", 4))
			{
				printf("It is a partition\n");
				partition_path = (char *)malloc(strlen(dir_path) + 6);
				strcpy(partition_path, dir_path);
				strcat(partition_path, "/");
				strcat(partition_path, dir_ent->d_name);
				return_value = check_for_user_data(partition_path);
				if (return_value == 1)
				{
					printf("Found a user partition, returning\n");
					closedir(dir);
					return partition_path;
				}
				free(partition_path);
				if (return_value == -1)
				{
					closedir(dir);
					return NULL;
				}
			}
		}
		closedir(dir);
		free(dir_path);
	}
	return NULL;
}

gboolean *mount_partitions(struct mount_info *mount_data)
{
	static gboolean retval = TRUE;

#ifdef DESTRUCTIVE
	if (mount(mount_data->snapptix, "/mnt", "ext3", 0, NULL))
		THREAD_EXIT(&retval);

	/* Make /var/lib/pkg directory for pkgutils */
	if (mkdir("/mnt/var", 0))
		THREAD_EXIT(&retval);
	if (mkdir("/mnt/var/lib", 0))
		THREAD_EXIT(&retval);
	if (mkdir("/mnt/var/lib/pkg", 0))
		THREAD_EXIT(&retval);

	/* Make db file for pkgutils */
	if (mknod("/mnt/var/lib/pkg/db", S_IFREG | 0644, 0))
		THREAD_EXIT(&retval);

	/* Make /home mountpoint */
	if (mkdir("/mnt/home", 0))
		THREAD_EXIT(&retval);
	if (mount(mount_data->user, "/mnt/home", "reiserfs", 0, NULL))
		THREAD_EXIT(&retval);

	/* Make /dev mountpoint and mount dev with bind */
	if (mkdir("/mnt/dev", 0))
		THREAD_EXIT(&retval);
	if (mount("/dev", "/mnt/dev", NULL, MS_BIND, NULL))
		THREAD_EXIT(&retval);

	/* Make /proc mountpoint and mount it with bind */
	if (mkdir("/mnt/proc", 0))
		THREAD_EXIT(&retval);
	if (mount("/proc", "/mnt/proc", NULL, MS_BIND, NULL))
		THREAD_EXIT(&retval);

#endif
	retval = FALSE;
	THREAD_EXIT(&retval);
}

gboolean *unmount_partitions()
{
	static gboolean retval = TRUE;

	/* This may not be mounted if we aren't
	 * alpha */
	umount("/mnt/home");

	if (umount("/mnt/dev"))
		THREAD_EXIT(&retval);
	if (umount("/mnt/proc"))
		THREAD_EXIT(&retval);
	if (umount("/mnt"))
		THREAD_EXIT(&retval);

	retval = FALSE;
	THREAD_EXIT(&retval);
}
