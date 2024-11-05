#include "gtk_install.h"

/* This is a collection of functions for working with threads */

/* This just sets the thread_finished flag with mutual
 * mutual exclusion */
void set_finished()
{
	pthread_mutex_lock(&mut);
	thread_finished = TRUE;
	pthread_mutex_unlock(&mut);
}

/* Sets the percent done and minor_label text with mutual exclusion */
void update_thread_info(gfloat current_percent, char *msg)
{
	pthread_mutex_lock(&mut);
	percent_done = current_percent;
	minor_label_text = msg;
	pthread_mutex_unlock(&mut);
}

/* Initiates the sets the finished flag,
 * initiates the mutex, and starts the thread */
void start_thread(void *function, void *data)
{
	thread_finished = FALSE;
	pthread_mutex_init(&mut, NULL);
	pthread_create(&tid, NULL, function, data);
}
