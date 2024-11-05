#include "gtk_install.h"
#include "packages.h"

/* calls pkgadd and checks the return value */
/* it will use alternate root if specified */
gboolean pkgadd(char *pkg_path,
		char *root)
{
	int pid;
	int status;

#ifdef DESTRUCTIVE
	pid = fork();
	if (pid < 0)
		return TRUE;
	if (pid) /* Parent */
	{
		waitpid(pid, &status, 0);
		if (status)
			return TRUE;
	}
	else /* Child */
	{
		/* NOTE: We FORCE these installs as some files conflict */
		/* in the first install */
		if (root)
			execlp("pkgadd", "pkgadd", "-f", "-r", root, pkg_path, NULL);
		else
			execlp("pkgadd", "pkgadd", "-f", pkg_path, NULL);
		exit(1);
	}
#endif
	return FALSE;
}

gboolean *install_base(GtkWidget *minor_label)
{
	gfloat 			num_of_packages = 0.0;
	gfloat 			current_package_num = 0.0;
	char 			*current_package;
	static gboolean 	retval = TRUE;
	char 			buff[BUFFSIZE];

	update_thread_info(0.0, NULL);

	for (current_package = base_packages;
	     strlen(current_package);
	     current_package += strlen(current_package) + 1)
		num_of_packages += 1.0;

	for (current_package = base_packages;
	     strlen(current_package);
	     current_package += strlen(current_package) + 1)
	{
		sprintf(buff, "Installing Package: %s", current_package);
		update_thread_info(current_package_num/num_of_packages, buff);
		/* End critical section */
		if (pkgadd(current_package, "/mnt"))
			THREAD_EXIT(&retval);
		current_package_num += 1.0;
	}
	retval = FALSE;
	THREAD_EXIT(&retval);
}
