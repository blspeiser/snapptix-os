#include "gtk_install.h"
#include "configs.h"

/* fonts */
char font_name[] = "-*-helvetica-medium-r-normal--24-*";
char minor_font_name[] = "-*-helvetica-medium-r-normal--12-*";

/* These are some colors various windows use */
static GdkColor grey = { 0, GREY_VALUE, GREY_VALUE, GREY_VALUE };
static GdkColor black = { 0, 0, 0, 0 };

/* Globals for threads */
char *minor_label_text = NULL;

char *data_partition;

void debug_output(char *msg)
{
#ifdef DEBUG
	FILE *fp;

	fp = fopen(DEBUG_LOG, "a+");
	fprintf(fp, msg);
	fclose(fp);
#endif
}

gboolean update_progress_bar(struct update_struct *update)
{
	gboolean *retval;
	char *curr_text;

	/* Critical Section */
	pthread_mutex_lock(&mut);
	if (update->progress_bar->activity_mode)
		/* As long as we set it to something different it will
		 * move */
		if (gtk_progress_get_value(update->progress_bar))
			gtk_progress_set_value(update->progress_bar, 0);
		else
			gtk_progress_set_value(update->progress_bar, 1);
	else
		gtk_progress_set_percentage((GtkProgress *)update->progress_bar, percent_done);
	if (minor_label_text)
	{
		gtk_label_get(GTK_LABEL(update->minor_label), &curr_text);
		if (strcmp(curr_text, minor_label_text))
			gtk_label_set_text(GTK_LABEL(update->minor_label), minor_label_text);
	}
	if (thread_finished)
	{
		pthread_mutex_unlock(&mut);
		pthread_join(tid, (void **)&retval);
		if (*retval) /*--ERROR--*/
			setup_window(NULL, &update->error_window);
		else /* Things are OK */
			setup_window(NULL, &update->next_window);
		return FALSE; /* End timer */
	}
	pthread_mutex_unlock(&mut);
	/* End critical Section */
	return TRUE;
}

/* General function to make sure an ip address is OK */
gboolean is_valid_ip(char *ip)
{
	int first, second, third, fourth;
	char dummy[64];

	if (strlen(ip) > 15)
		return FALSE;
	if (sscanf(ip, "%d.%d.%d.%d%s", &first, &second, &third, &fourth, dummy) != 4)
		return FALSE;
	if ((first > 255) || (first <= 0) ||
	    (second > 255) || (second < 0) ||
	    (third > 255) || (third < 0) ||
	    (fourth > 255) || (fourth <= 0))
		return FALSE;
	return TRUE;
}

void got_yes(GtkWidget *widget)
{
	static int data;

	gtk_widget_destroy(widget);
	data = CONTINUE_CHECK;
	setup_window(NULL, &data);
}

void got_no(GtkWidget *widget)
{
	static int data;

	gtk_widget_destroy(widget);
	/* Set data_partition to NULL first */
	data_partition = NULL;
	data = CONTINUE_CHECK; 
	setup_window(NULL, &data);
}

void previous_install_query(int type)
{
	GtkWidget *dialog;
	GtkWidget *frame;
	GtkWidget *label;
	GtkWidget *yes_button;
	GtkWidget *no_button;
	GdkPixmap *image_data;
	GdkBitmap *mask;
	GtkWidget *pixmap;
	GtkWidget *button_alignment;
	GdkCursor *cursor;
	GtkWidget *button_hbox;
	GtkWidget *vbox;

	dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), BORDER_WIDTH);

	/* Set the default mouse cursor */
	gtk_widget_realize(dialog);
	/* Associate the window with X resources */
	cursor = gdk_cursor_new(GDK_ARROW);
	gdk_window_set_cursor(dialog->window, cursor);
	gdk_cursor_destroy(cursor);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(dialog), vbox);

	image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, HEADER_IMAGE);
	pixmap = gtk_pixmap_new(image_data, mask);
	gtk_widget_show(pixmap);
	gtk_box_pack_start(GTK_BOX(vbox), pixmap, FALSE, FALSE, 0);

	frame = gtk_frame_new ("Previous install detected");
	gtk_widget_show(frame);
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame), BORDER_WIDTH);

	button_alignment = gtk_alignment_new(0.5, 0.5, 0.5, 1);
	gtk_widget_show(button_alignment);
	gtk_box_pack_start(GTK_BOX(vbox), button_alignment, FALSE, FALSE, 0);

	switch (type)
	{
		case QUERY_REPAIR:
			label = gtk_label_new(PREVIOUS_INSTALL_TEXT);
			gtk_widget_show(label);
			gtk_container_add(GTK_CONTAINER(frame), label);

			button_hbox = gtk_hbox_new(FALSE, 0);
			gtk_widget_show(button_hbox);
			gtk_container_add(GTK_CONTAINER(button_alignment), button_hbox);

			yes_button = gtk_button_new_with_label("YES");
			gtk_widget_show(yes_button);
			gtk_signal_connect_object(GTK_OBJECT(yes_button), "clicked",
			  	  		  GTK_SIGNAL_FUNC(got_yes),
			  	  		  GTK_OBJECT(dialog));
			gtk_box_pack_start(GTK_BOX(button_hbox), yes_button, TRUE, TRUE, 0);

			no_button = gtk_button_new_with_label("NO");
			gtk_widget_show(no_button);
			gtk_signal_connect_object(GTK_OBJECT(no_button), "clicked",
			  	  		  GTK_SIGNAL_FUNC(got_no),
			  	  		  GTK_OBJECT(dialog));
			gtk_box_pack_start(GTK_BOX(button_hbox), no_button, TRUE, TRUE, 0);
		break;
		case QUERY_ERROR:
			label = gtk_label_new(CORRUPT_PARTITION_TEXT);
			gtk_widget_show(label);
			gtk_container_add(GTK_CONTAINER(frame), label);

			no_button = gtk_button_new_with_label("Next");
			gtk_widget_show(no_button);
			gtk_signal_connect_object(GTK_OBJECT(no_button), "clicked",
			  	  		  GTK_SIGNAL_FUNC(got_no),
			  	  		  GTK_OBJECT(dialog));
			gtk_box_pack_start(GTK_BOX(button_hbox), no_button, TRUE, TRUE, 0);
		break;
	}

	gtk_window_set_default_size(GTK_WINDOW(dialog), WINDOW_WIDTH, WINDOW_HEIGHT);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_widget_show(dialog);
}

/* Creates a window displaying an error */
/* prompt determines whether or not you want an */
/* ok button to continue */
void error_msg(char *message, gboolean prompt)
{
	GtkWidget *dialog;
	GtkWidget *frame;
	GtkWidget *label;
	GtkWidget *okay_button;
	GdkPixmap *image_data;
	GdkBitmap *mask;
	GtkWidget *pixmap;
	GtkWidget *button_alignment;
	GdkCursor *cursor;

	dialog = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER(dialog), BORDER_WIDTH);

	/* Set the default mouse cursor */
	gtk_widget_realize(dialog);
	/* Associate the window with X resources */
	cursor = gdk_cursor_new(GDK_ARROW);
	gdk_window_set_cursor(dialog->window, cursor);
	gdk_cursor_destroy(cursor);

	image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, HEADER_IMAGE);
	pixmap = gtk_pixmap_new(image_data, mask);
	gtk_widget_show(pixmap);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), pixmap, FALSE, FALSE, 0);

	frame = gtk_frame_new ("Error");
	gtk_widget_show(frame);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame), BORDER_WIDTH);

	label = gtk_label_new(message);
	gtk_widget_show(label);
	gtk_container_add(GTK_CONTAINER(frame), label);

	if (prompt)
	{
		button_alignment = gtk_alignment_new(0.5, 0.5, 0.5, 1);
		gtk_widget_show(button_alignment);
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), button_alignment, FALSE, FALSE, 0);

		okay_button = gtk_button_new_with_label("OK");
		gtk_widget_show(okay_button);
		gtk_signal_connect_object(GTK_OBJECT(okay_button), "clicked",
				  	  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  	  GTK_OBJECT(dialog));
		gtk_container_add(GTK_CONTAINER(button_alignment), okay_button);
	}

	gtk_window_set_default_size(GTK_WINDOW(dialog), WINDOW_WIDTH, WINDOW_HEIGHT);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_widget_show(dialog);
}

/* Quick little function to update the progress bar in our check_network_config
 * window */
gboolean network_config_update(struct update_network_struct *update_network)
{
	gboolean *retval;

	/* Critical Section */
	pthread_mutex_lock(&mut);
	/* As long as we set it to something different it will
	 * move */
	if (gtk_progress_get_value(update_network->progress_bar))
		gtk_progress_set_value(update_network->progress_bar, 0);
	else
		gtk_progress_set_value(update_network->progress_bar, 1);
	if (thread_finished)
	{
		pthread_mutex_unlock(&mut);
		pthread_join(tid, (void **)&retval);
		update_network->network_data->current_section = CHECK_EXIT;
		check_network_config(NULL, update_network->network_data);
		return FALSE; /* End timer */
	}
	pthread_mutex_unlock(&mut);
	/* End critical section */
	return TRUE;
}

/* Callback to make sure network config data is valid */
void check_network_config(GtkWidget *widget, struct network_info *network_data)
{
	static GtkWidget *net_detection;
	static GtkWidget *net_detection_vbox;
	static GtkWidget *pixmap;
	static GtkWidget *net_detection_frame;
	static GtkWidget *net_detection_label;
	static GtkWidget *button_alignment;
	static GtkWidget *next;
	static GdkPixmap *image_data;
	static GdkBitmap *mask;
	static int detected_device;
	static int data;
	static struct find_connected_struct find_connected;
	static struct update_network_struct update_network;
	static GtkWidget *progress_bar;
	static GtkWidget *pixmap_fixed;
	static GtkRcStyle *rc_style;
	static GtkWidget *pixmap_description;
	static GtkWidget *progress_bar_alignment;
	static GdkCursor *cursor;
	char str[BUFFSIZE];

	switch (network_data->current_section)
	{
		case CHECK_ENTRIES: 
			if (!is_valid_ip(gtk_entry_get_text(GTK_ENTRY(network_data->ip_entry))))
			{
				error_msg("Invalid IP Address", TRUE);
				return;
			}
			if (!is_valid_ip(gtk_entry_get_text(GTK_ENTRY(network_data->cluster_ip_entry))))
			{
				error_msg("Invalid Cluster IP Address", TRUE);
				return;
			}
			if (!strlen(gtk_entry_get_text(GTK_ENTRY(network_data->primary_hostname_entry))))
			{
				error_msg("Please enter a Primary Hostname", TRUE);
				return;
			}
			if (!strlen(gtk_entry_get_text(GTK_ENTRY(network_data->secondary_hostname_entry))))
			{
				error_msg("Please enter a Secondary Hostname", TRUE);
				return;
			}
			if (!is_valid_ip(gtk_entry_get_text(GTK_ENTRY(network_data->dns1_entry))))
			{
				error_msg("Invalid Primary DNS entry", TRUE);
				return;
			}
			if (!strcmp(gtk_entry_get_text(GTK_ENTRY(network_data->primary_hostname_entry)),
		    		    gtk_entry_get_text(GTK_ENTRY(network_data->secondary_hostname_entry))))
			{
				error_msg("The entered hostnames must differ", TRUE);
				return;
			}
			if (!strcmp(gtk_entry_get_text(GTK_ENTRY(network_data->ip_entry)),
		    		    gtk_entry_get_text(GTK_ENTRY(network_data->cluster_ip_entry))))
			{
				error_msg("The IP address must differ from the cluster address", TRUE);
				return;
			}

			net_detection = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			gtk_container_set_border_width(GTK_CONTAINER(net_detection), BORDER_WIDTH);
			gtk_window_set_title(GTK_WINDOW(net_detection), "Net Detection Window");
			gtk_window_set_default_size(GTK_WINDOW(net_detection), WINDOW_WIDTH, WINDOW_HEIGHT);

			/* Set the default mouse cursor */
			gtk_widget_realize(net_detection);
			/* Associate the window with X resources */
			cursor = gdk_cursor_new(GDK_ARROW);
			gdk_window_set_cursor(net_detection->window, cursor);
			gdk_cursor_destroy(cursor);

			net_detection_vbox = gtk_vbox_new (FALSE, 0);
			gtk_widget_show(net_detection_vbox);
			gtk_container_add (GTK_CONTAINER(net_detection), net_detection_vbox);

			image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, HEADER_IMAGE);
			pixmap = gtk_pixmap_new(image_data, mask);
			gtk_widget_show (pixmap);
			gtk_box_pack_start (GTK_BOX(net_detection_vbox), pixmap, FALSE, FALSE, 0);

			net_detection_frame = gtk_frame_new ("Network Detection");
			gtk_widget_show (net_detection_frame);
			gtk_box_pack_start (GTK_BOX(net_detection_vbox), net_detection_frame, TRUE, TRUE, 0);
			gtk_container_set_border_width (GTK_CONTAINER (net_detection_frame), 10);

			net_detection_label = gtk_label_new ("Please disconnect all networking cables from\nfrom the computer.  Then only connect the\nexternal interface.");
			gtk_widget_show (net_detection_label);
			gtk_container_add (GTK_CONTAINER (net_detection_frame), net_detection_label);

			button_alignment = gtk_alignment_new (0.5, 0.5, 0.5, 1);
			gtk_widget_show (button_alignment);
			gtk_box_pack_start (GTK_BOX(net_detection_vbox), button_alignment, FALSE, FALSE, 0);

			next = gtk_button_new_with_label ("Next");
			gtk_widget_show (next);
			gtk_container_add (GTK_CONTAINER (button_alignment), next);

			gtk_widget_show(net_detection);

			network_data->current_section = CHECK_NET;
			gtk_signal_connect(GTK_OBJECT(next), "clicked", GTK_SIGNAL_FUNC(check_network_config), network_data);
		break;
		case CHECK_NET:
			gtk_widget_destroy(next);
			gtk_widget_destroy(pixmap);
			gtk_widget_destroy(net_detection_label);
			gtk_widget_destroy(net_detection_frame);
		
			pixmap_fixed = gtk_fixed_new();
			/* Make the background black (labels get their background from the parent) */
			rc_style = gtk_rc_style_new();
			rc_style->bg[GTK_STATE_NORMAL] = black;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_BG;
			gtk_widget_modify_style(pixmap_fixed, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show(pixmap_fixed);
			gtk_box_pack_start(GTK_BOX(net_detection_vbox), pixmap_fixed, TRUE, FALSE, 0);
			gtk_widget_set_usize(pixmap_fixed, MAIN_PIXMAP_WIDTH, MAIN_PIXMAP_HEIGHT);

			image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, BACKGROUND_IMAGE);
			pixmap = gtk_pixmap_new(image_data, mask);
			gtk_widget_show (pixmap);
			gtk_fixed_put (GTK_FIXED(pixmap_fixed), pixmap, 0, 0);
			gtk_widget_set_uposition(pixmap, 0, 0);
			gtk_misc_set_alignment(GTK_MISC(pixmap), 0, 0);

			pixmap_description = gtk_label_new(NETDETECT_TEXT);
			/* Make the label grey */
			rc_style = gtk_rc_style_new();
			rc_style->fg[GTK_STATE_NORMAL] = grey;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_FG;
			free(rc_style->font_name);
			rc_style->font_name = malloc(strlen(font_name));
			strcpy(rc_style->font_name, font_name);
			gtk_widget_modify_style(pixmap_description, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show (pixmap_description);
			gtk_fixed_put (GTK_FIXED (pixmap_fixed), pixmap_description, 22, 270);
			gtk_widget_set_uposition (pixmap_description, 22, 270);
			gtk_label_set_justify (GTK_LABEL (pixmap_description), GTK_JUSTIFY_LEFT);
			gtk_misc_set_alignment (GTK_MISC (pixmap_description), 0, 0);

			/* Create an alignment for our progress bar */
			progress_bar_alignment = gtk_alignment_new (0.5, 0.5, 1, 0.5);
			gtk_widget_show(progress_bar_alignment);
			gtk_box_pack_start(GTK_BOX(net_detection_vbox), progress_bar_alignment, TRUE, TRUE, 0);

			/* Create our progress bar */
			progress_bar = gtk_progress_bar_new();
			gtk_widget_show(progress_bar);
			gtk_container_add(GTK_CONTAINER(progress_bar_alignment), progress_bar);

			/* Make the progress bar bounce back and forth */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, TRUE);
			gtk_progress_bar_set_activity_step((GtkProgressBar *)progress_bar, PROGBARINC);

			debug_output("Checking network parameters\n");

			detected_device = -1;
			find_connected.ip = gtk_entry_get_text(GTK_ENTRY(network_data->ip_entry));
			find_connected.dns1 = gtk_entry_get_text(GTK_ENTRY(network_data->dns1_entry));
			find_connected.dns2 = gtk_entry_get_text(GTK_ENTRY(network_data->dns2_entry));
			find_connected.netmask = "";
			find_connected.broadcast = "";
			find_connected.gateway = gtk_entry_get_text(GTK_ENTRY(network_data->gateway_entry));
			find_connected.detected_device = &detected_device;
			find_connected.check_devices = 2;

			/* Start a thread */
			start_thread((void *)find_connected_external, (void *)&find_connected);

			/* Start our update timer */
			update_network.network_data = network_data;
			update_network.progress_bar = (GtkProgress *)progress_bar;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)network_config_update, &update_network);
		break;
		case CHECK_EXIT:
			if (detected_device < 0)
			{
				/*--ERROR--*/
				gtk_widget_destroy(net_detection);
				network_data->current_section = CHECK_ENTRIES;
				error_msg("Unable to confirm interface functions.\nMake sure your parameters are correct.", TRUE);
			}
			else
			{
				strcpy(network_data->ip, gtk_entry_get_text(GTK_ENTRY(network_data->ip_entry)));
				strcpy(network_data->primary_hostname, gtk_entry_get_text(GTK_ENTRY(network_data->primary_hostname_entry)));
				strcpy(network_data->secondary_hostname, gtk_entry_get_text(GTK_ENTRY(network_data->secondary_hostname_entry)));
				strcpy(network_data->cluster_ip, gtk_entry_get_text(GTK_ENTRY(network_data->cluster_ip_entry)));
				if (strlen(gtk_entry_get_text(GTK_ENTRY(network_data->gateway_entry))))
					strcpy(network_data->gateway, gtk_entry_get_text(GTK_ENTRY(network_data->gateway_entry)));
				else
					strcpy(network_data->gateway, SNAPPTIX_FILL);
				if (strlen(gtk_entry_get_text(GTK_ENTRY(network_data->subnet_entry))))
					strcpy(network_data->subnet, gtk_entry_get_text(GTK_ENTRY(network_data->subnet_entry)));
				else
					strcpy(network_data->subnet, SNAPPTIX_FILL);
				if (strlen(gtk_entry_get_text(GTK_ENTRY(network_data->dns1_entry))))
					strcpy(network_data->dns1, gtk_entry_get_text(GTK_ENTRY(network_data->dns1_entry)));
				else
					strcpy(network_data->dns1, SNAPPTIX_FILL);
				if (strlen(gtk_entry_get_text(GTK_ENTRY(network_data->dns2_entry))))
					strcpy(network_data->dns2, gtk_entry_get_text(GTK_ENTRY(network_data->dns2_entry)));
				else
					strcpy(network_data->dns2, SNAPPTIX_FILL);
				sprintf(str, "eth%d", detected_device);
				strcpy(network_data->public_device, str);
				if (network_data->devices == 1) /* if this is an alpha only install */
				{
					strcpy(network_data->private_device, "dummy0");
				}
				else
				{
					if (detected_device)
						strcpy(network_data->private_device, "eth0");
					else
						strcpy(network_data->private_device, "eth1");
				}
				debug_output("Detected public device\n");
				debug_output(network_data->public_device);
				debug_output("\nSetting private device\n");
				debug_output(network_data->private_device);
				debug_output("\n");
				gtk_widget_destroy(net_detection);
				data = SETUP_CONFIGS;
				setup_window(NULL, &data);
			}
		break;
	}
}

/* Callback for when a listbox is selected */
void selection_made(GtkWidget *clist, gint row, gint column,
		    GdkEventButton *button, struct selected_struct *selected)
{
	*selected->row_selected = row;
	gtk_widget_set_sensitive(selected->next, TRUE);
}

/* Callback to make sure the passwords are the same */
void check_passwords(GtkWidget *widget, struct pwinfo *pwdata)
{
	char message[BUFFSIZE];
	static int data;

	if (!strlen(gtk_entry_get_text(pwdata->entry1)))
		strcpy(message, "Please enter a password");
	else
	if (strcmp(gtk_entry_get_text(pwdata->entry1),
	           gtk_entry_get_text(pwdata->entry2)))
		strcpy(message, "Entries do not match");
	else
	{
		strcpy(pwdata->password, gtk_entry_get_text(pwdata->entry1));
		data = NETWORK_CONFIG;
		setup_window(NULL, &data);
		return;
	}
	/* Clear the entries */
	gtk_entry_set_text(pwdata->entry1, "");
	gtk_entry_set_text(pwdata->entry2, "");

	/* Give an error message */
	error_msg(message, TRUE);
}

void setup_window(GtkWidget *widget,
  		  gpointer data)
{
	static GtkWidget *gtk_install_window;
	static GtkWidget *main_vbox;
	static GtkWidget *header_pixmap;
	static GtkWidget *network_config_hbox;
	static GtkWidget *network_config_vbox;
	static GtkWidget *cluster_frame_alignment;
	static GtkWidget *cluster_ip_frame;
	static GtkWidget *cluster_frame_vbox;
	static GtkWidget *cluster_ip_hbox;
	static GtkWidget *cluster_ip_label;
	static GtkWidget *cluster_ip_entry;
	static GtkWidget *node_hbox;
	static GtkWidget *primary_frame;
	static GtkWidget *primary_table;
	static GtkWidget *primary_ip_label;
	static GtkWidget *primary_hostname_label;
	static GtkWidget *primary_hostname_entry;
	static GtkWidget *primary_subnet_label;
	static GtkWidget *primary_subnet_entry;
	static GtkWidget *primary_ip_entry;
	static GtkWidget *primary_gateway_label;
	static GtkWidget *primary_gateway_entry;
	static GtkWidget *primary_dns1_label;
	static GtkWidget *primary_dns1_entry;
	static GtkWidget *primary_dns2_label;
	static GtkWidget *primary_dns2_entry;
	static GtkWidget *secondary_frame;
	static GtkWidget *secondary_table;
	static GtkWidget *secondary_hostname_label;
	static GtkWidget *secondary_ip_label;
	static GtkWidget *secondary_subnet_label;
	static GtkWidget *secondary_hostname_entry;
	static GtkWidget *secondary_ip_entry;
	static GtkWidget *secondary_subnet_entry;
	static GtkWidget *secondary_gateway_label;
	static GtkWidget *secondary_gateway_entry;
	static GtkWidget *secondary_dns1_label;
	static GtkWidget *secondary_dns1_entry;
	static GtkWidget *secondary_dns2_label;
	static GtkWidget *secondary_dns2_entry;
	static GtkWidget *button_alignment;
	static GtkWidget *pixmap_fixed;
	static GtkWidget *pixmap_description;
	static GtkWidget *bottom_separator_alignment;
	static GtkWidget *bottom_separator;
	static GdkPixmap *image_data;
	static GdkBitmap *mask;
	static GtkWidget *main_pixmap;
	static GtkRcStyle *rc_style;
	static GtkWidget *progress_bar_alignment;
	static GtkWidget *node_type_frame;
	static GtkWidget *list_alignment;
	static GtkWidget *node_type_list;
	static GtkWidget *timezone_select_list;
	static GtkWidget *password_vbox;
	static GtkWidget *password_prompt_label;
	static GtkWidget *password_table;
	static GtkWidget *password1_label;
	static GtkWidget *password2_label;
	static GtkWidget *password1_entry;
	static GtkWidget *password2_entry;
	static GtkStyle *default_style;
	static GtkWidget *sync_label;
	static GdkCursor		*cursor;
	static GtkWidget 		*next;
	static GtkWidget 		*progress_bar;
	static GtkWidget		*minor_label;
	static struct discs 		*root = NULL;
	static struct mount_info	mount_data;
	static char			str[BUFFSIZE];
	static struct update_struct	update;
	static struct selected_struct	selected;
	static struct setup_configs_struct setup_info;
	static struct pwinfo		pwdata;
	static struct network_info	network_data;
	static struct sync_with_alpha_struct sync_struct;
	static int 			timezone_selected;
	unsigned short			str_size;

	/* Timezone information */
	gchar *timezone_title[] = { "Please select a timezone" };
#define TZROWS 4
	gchar *timezones[][TZROWS] = { { "Eastern Standard Time" },
				      { "Central Standard Time" },
				      { "Pacific Standard Time" },
				      { "Mountain Standard Time" } };

	/* Network config information */
	gchar *node_type_title[] = { "Please select Node Type" };
	gchar *node_type_alpha[] = { "Primary" };
	gchar *node_type_beta[] = { "Secondary" };

	switch (*(int *)data)
	{
		case INTRO_WIN:
			/* Set the default font */
			default_style = gtk_style_new();
			gdk_font_unref(default_style->font);
			default_style->font = gdk_font_load(font_name);
			gtk_widget_set_default_style(default_style);
			gtk_style_unref(default_style);

			/* Adapted from Glade */
			gtk_install_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
			gtk_window_set_title (GTK_WINDOW (gtk_install_window), "Snapptix Install");
			gtk_window_set_default_size (GTK_WINDOW (gtk_install_window), WINDOW_WIDTH, WINDOW_HEIGHT);

			main_vbox = gtk_vbox_new (FALSE, 0);
			gtk_widget_show (main_vbox);
			gtk_container_add (GTK_CONTAINER (gtk_install_window), main_vbox);
			gtk_container_set_border_width (GTK_CONTAINER (main_vbox), BORDER_WIDTH);

			pixmap_fixed = gtk_fixed_new ();
			/* Make the background black (labels get their background from the parent) */
			rc_style = gtk_rc_style_new();
			rc_style->bg[GTK_STATE_NORMAL] = black;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_BG;
			gtk_widget_modify_style(pixmap_fixed, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show (pixmap_fixed);
			gtk_box_pack_start (GTK_BOX (main_vbox), pixmap_fixed, TRUE, FALSE, 0);
			gtk_widget_set_usize (pixmap_fixed, MAIN_PIXMAP_WIDTH, MAIN_PIXMAP_HEIGHT);

			image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, BACKGROUND_IMAGE);
			main_pixmap = gtk_pixmap_new(image_data, mask);
			gtk_widget_show (main_pixmap);
			gtk_fixed_put (GTK_FIXED (pixmap_fixed), main_pixmap, 0, 0);
			gtk_widget_set_uposition (main_pixmap, 0, 0);
			gtk_misc_set_alignment (GTK_MISC (main_pixmap), 0, 0);

			pixmap_description = gtk_label_new (WELCOME_TEXT);
			/* Make the label grey */
			rc_style = gtk_rc_style_new();
			rc_style->fg[GTK_STATE_NORMAL] = grey;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_FG;
			free(rc_style->font_name);
			rc_style->font_name = malloc(strlen(font_name));
			strcpy(rc_style->font_name, font_name);
			gtk_widget_modify_style(pixmap_description, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show (pixmap_description);
			gtk_fixed_put (GTK_FIXED (pixmap_fixed), pixmap_description, 22, 270);
			gtk_widget_set_uposition (pixmap_description, 22, 270);
			gtk_widget_set_usize (pixmap_description, 0, 0);
			gtk_label_set_justify (GTK_LABEL (pixmap_description), GTK_JUSTIFY_LEFT);
			gtk_misc_set_alignment (GTK_MISC (pixmap_description), 0, 0);

			bottom_separator_alignment = gtk_alignment_new (0.5, 0.5, 1, 0.2);
			gtk_widget_show (bottom_separator_alignment);
			gtk_box_pack_start (GTK_BOX (main_vbox), bottom_separator_alignment, FALSE, TRUE, 0);
			gtk_widget_set_usize (bottom_separator_alignment, -2, 80);

			bottom_separator = gtk_hseparator_new();
			gtk_widget_show (bottom_separator);
			gtk_container_add (GTK_CONTAINER (bottom_separator_alignment), bottom_separator);

			button_alignment = gtk_alignment_new (0.5, 0.5, 0.5, 1);
			gtk_widget_show (button_alignment);
			gtk_box_pack_start (GTK_BOX (main_vbox), button_alignment, TRUE, FALSE, 0);

			next = gtk_button_new_with_label ("Next");
			gtk_widget_show (next);
			gtk_container_add (GTK_CONTAINER (button_alignment), next);

			/* Set the default mouse cursor */
			gtk_widget_realize(gtk_install_window);
			/* Associate the window with X resources */
			cursor = gdk_cursor_new(GDK_ARROW);
			gdk_window_set_cursor(gtk_install_window->window, cursor);
			gdk_cursor_destroy(cursor);

			*(int *)data = CHECK_DRIVE_WIN;
			gtk_signal_connect(GTK_OBJECT(next), "clicked", GTK_SIGNAL_FUNC(setup_window), data);

			/* Show main window */
			gtk_widget_show(gtk_install_window);
		break;
		case CHECK_DRIVE_WIN:
			/* Remove the old stuff */
			gtk_widget_destroy(next);
			gtk_widget_destroy(button_alignment);
			gtk_widget_destroy(bottom_separator);
			gtk_widget_destroy(bottom_separator_alignment);

			/* Set the harddrive check text */
			gtk_label_set_text(GTK_LABEL(pixmap_description), HDCHECK_TEXT);

			/* Setup the minor label */
			minor_label = gtk_label_new("Running badblocks");
			gtk_label_set_justify(GTK_LABEL(minor_label), GTK_JUSTIFY_LEFT);
			gtk_misc_set_alignment(GTK_MISC(minor_label), 0, 0);

			/* Make sure the font is smaller */
			rc_style = gtk_rc_style_new();
			free(rc_style->font_name);
			rc_style->font_name = malloc(strlen(minor_font_name));
			strcpy(rc_style->font_name, minor_font_name);
			gtk_widget_modify_style(minor_label, rc_style);
			gtk_widget_show(minor_label);

			/* Stick it in the box */
			gtk_box_pack_start(GTK_BOX(main_vbox), minor_label, FALSE, FALSE, 0);

			/* Create an alignment for our progress bar */
			progress_bar_alignment = gtk_alignment_new (0.5, 0.5, 1, 0.5);
			gtk_widget_show(progress_bar_alignment);
			gtk_box_pack_start(GTK_BOX(main_vbox), progress_bar_alignment, TRUE, TRUE, 0);

			/* Create our progress bar */
			progress_bar = gtk_progress_bar_new();
			gtk_widget_show(progress_bar);
			gtk_container_add(GTK_CONTAINER(progress_bar_alignment), progress_bar);

			/* Find the harddrive */
			find_discs("/dev/ide", &root);
			if (!root)
			{
				/*--ERROR--*/
				error_msg(HARDDRIVE_ERROR_TEXT, FALSE);
				break;
			}
			else
			{
				debug_output("Found hard drive ");
				debug_output(root->path);
				debug_output("\n");
			}
			strcpy(mount_data.root_device, root->path);

			/* Create our snapptix partition string */
			str_size = strlen(root->path) - 4;
			strncpy(mount_data.snapptix, root->path, str_size);
			strcpy(mount_data.snapptix + str_size, "part2");

			/* Create our user partition string */
			str_size = strlen(root->path) - 4;
			strncpy(mount_data.user, root->path, str_size);
			strcpy(mount_data.user + str_size, "part3");

			/* Check for a snapptix data partition */
			data_partition = find_snapptix_partition(root);
			if (data_partition)
			{
				debug_output("Found user data partition ");
				debug_output(data_partition);
				debug_output("\n");
				if (strcmp(data_partition + strlen(data_partition) - 5, "part3"))
				{
					debug_output("Partition table corrupt\n");
					debug_output(data_partition + strlen(data_partition) - 5);
					previous_install_query(QUERY_ERROR);
					/* Tell em' their out of luck for recovery */
				}
				else
				if (get_hdsize(mount_data.snapptix) < SNAPPTIX_SIZE)
				{
					debug_output("Snapptix partition too small\n");
					previous_install_query(QUERY_ERROR);
					/* Tell em' their out of luck for recovery */
				}
				else
				{
					/* See if they want to recover */
					previous_install_query(QUERY_REPAIR);
					break;
				}
			}
			else
				debug_output("Didn't find Snapptix data partition\n");
		case CONTINUE_CHECK:
			/* Check harddrive integrity */

			/* Start a thread */
			start_thread((void *)bad_block_scan, (void *)root->path);

			/* Start our update timer */
			update.next_window = NETWORK_DETECT;
			update.error_window = BAD_BLOCKS_ERROR_WIN;
			update.progress_bar = (GtkProgress *)progress_bar;
			update.minor_label = minor_label;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case BAD_BLOCKS_ERROR_WIN:
			gtk_widget_destroy(progress_bar);
			/*--ERROR--*/
			debug_output("Badblocks found on harddrive\n");
			error_msg(BAD_BLOCKS_ERROR_TEXT, FALSE);
		break;
		case NETWORK_DETECT:
			/* Change the text we're displaying */
			gtk_label_set_text(GTK_LABEL(pixmap_description), INSTALLING_TEXT);
			gtk_label_set_text(GTK_LABEL(minor_label), "Detecting network adapters");

			/* Make the progress bar bounce back and forth */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, TRUE);
			gtk_progress_bar_set_activity_step((GtkProgressBar *)progress_bar, PROGBARINC);

			debug_output("Detecting network adapters\n");

			start_thread((void *)detect_nic_cards, &network_data);

			/* Start our update timer */
			update.next_window = CREATE_PARTITIONS;
			update.error_window = NETWORK_DETECT_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case NETWORK_DETECT_ERROR:
			/*--ERROR--*/
			debug_output("Error detecting network adapters\n");
			error_msg(NETDETECT_ERROR_TEXT, FALSE);
		break;
		case CREATE_PARTITIONS:
			/* Check NIC stuff first */
			if (network_data.devices < 1)
			{
				error_msg(NONETDEVICE_ERROR_TEXT, FALSE);
				break;
			}
			if (network_data.devices == 1)
			{
				network_data.modules[1] = (char *)malloc(6);
				strcpy(network_data.modules[1], "dummy");
				/* Tell them this is an alpha only install */
			}
			sprintf(str, "Found %d nics:\n", network_data.devices);
			debug_output(str);
			if (network_data.modules[0] != NULL)
			{
				debug_output(network_data.modules[0]);
				debug_output("\n");
			}
			if (network_data.modules[1] != NULL)
			{
				debug_output(network_data.modules[1]);
				debug_output("\n");
			}

			/* Make the partitions */
			debug_output("Creating partitions\n");
			gtk_label_set_text(GTK_LABEL(minor_label), "Creating Partitions");

			/* Start a thread */
			start_thread((void *)create_partitions, (void *)root->path);

			/* Start our update timer */
			update.next_window = MAKE_SWAP;
			update.error_window = CREATE_PARTITIONS_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case CREATE_PARTITIONS_ERROR:
			/*--ERROR--*/
			debug_output("Error creating partitions\n");
		break;
		case MAKE_SWAP:
			/* Make the swap partition */
			gtk_label_set_text(GTK_LABEL(minor_label), "Making swap partition");

			/* Make our swap partition string (mkswap) */
			str_size = strlen(root->path) - 4;
			strncpy(mount_data.swap, root->path, str_size);
			strcpy(mount_data.swap + str_size, "part1");

			debug_output("Creating swap partition ");
			debug_output(mount_data.swap);
			debug_output("\n");

			/* Start a thread */
			start_thread((void *)make_swap, (void *)mount_data.swap);

			/* Start our update timer */
			update.next_window = MAKE_SNAPPTIX;
			update.error_window = MAKE_SWAP_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case MAKE_SWAP_ERROR:
			/*--ERROR--*/
			debug_output("Error making swap\n");
		break;
		case MAKE_SNAPPTIX:
			/* Make our snapptix root partition string */
			gtk_label_set_text(GTK_LABEL(minor_label), "Making SNAPPTIX partition");
			 
			debug_output("Creating SNAPPTIX partition ");
			debug_output(mount_data.snapptix);
			debug_output("\n");

			/* Start a thread */
			start_thread((void *)make_snapptix, (void *)mount_data.snapptix);

			/* don't erase the user partition if they want to keep it */
			if (data_partition)
			{
				update.next_window = MOUNT_PARTITIONS;
				update.error_window = MOUNT_PARTITIONS_ERROR;
			}
			else
			{
				update.next_window = MAKE_USER;
				update.error_window = MAKE_SNAPPTIX_ERROR;
			}
			/* Start our update timer */
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case MAKE_SNAPPTIX_ERROR:
			/*--ERROR--*/
			debug_output("Error while making SNAPPTIX\n");
		break;
		case MAKE_USER:
			/* Make the user partition */
			gtk_label_set_text(GTK_LABEL(minor_label), "Making user partition");

			debug_output("Creating user partition ");
			debug_output(mount_data.user);
			debug_output("\n");

			/* use mkreiserfs to make a user partition, with progress bar */
			/* switch progress bar back to percentage display */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, FALSE);

			/* Start a thread */
			start_thread((void *)make_user, (void *)mount_data.user);
			
			/* Start our update timer */
			update.next_window = MOUNT_PARTITIONS;
			update.error_window = MAKE_USER_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case MAKE_USER_ERROR:
			/*--ERROR--*/
			debug_output("Error while making user partition\n");
			error_msg(MAKE_USER_ERROR_TEXT, FALSE);
		break;
		case MOUNT_PARTITIONS:
			/* Switch back to activity display */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, TRUE);

			gtk_label_set_text(GTK_LABEL(minor_label), "Mounting partitions");
			debug_output("Mounting Partitions\n");
			
			/* Start a thread */
			start_thread((void *)mount_partitions, (void *)&mount_data);

			/* Start our update timer */
			update.next_window = INSTALL_PACKAGES;
			update.error_window = MOUNT_PARTITIONS_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case MOUNT_PARTITIONS_ERROR:
			/*--ERROR--*/
			debug_output("Error while mounting partitions\n");
		break;
		case INSTALL_PACKAGES:
			/* Switch to percentage display */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, FALSE);

			debug_output("Installing packages\n");

			/* Start a thread */
			start_thread((void *)install_base, (void *)minor_label);

			/* Start our update timer */
			update.next_window = INSTALL_NICS;
			update.error_window = INSTALL_PACKAGES_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case INSTALL_PACKAGES_ERROR:
			/*--ERROR--*/
			debug_output("Error while installing packages\n");
		break;
		case INSTALL_NICS:
			/* Switch to activity display */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, TRUE);

			gtk_label_set_text(GTK_LABEL(minor_label), "Installing network modules");
			debug_output("Installing network modules\n");

			/* Start a thread */
			start_thread((void *)install_nic_modules, &network_data);

			/* Start our update timer */
			update.next_window = NODE_TYPE_SELECT;
			update.error_window = INSTALL_NICS_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case INSTALL_NICS_ERROR:
			/*--ERROR--*/
			debug_output("Error while installing network modules\n");
		break;
		case NODE_TYPE_SELECT:
			/* Destroy old stuff */
			gtk_widget_destroy(main_pixmap);
			gtk_widget_destroy(pixmap_description);
			gtk_widget_destroy(pixmap_fixed);
			gtk_widget_destroy(progress_bar);
			gtk_widget_destroy(progress_bar_alignment);
			gtk_widget_destroy(minor_label);

			/* Adapted from glade */
			image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, HEADER_IMAGE);
			header_pixmap = gtk_pixmap_new(image_data, mask);
			gtk_widget_show (header_pixmap);
			gtk_box_pack_start (GTK_BOX (main_vbox), header_pixmap, FALSE, FALSE, 0);

			node_type_frame = gtk_frame_new ("Node Type");
			gtk_widget_show (node_type_frame);
			gtk_box_pack_start (GTK_BOX (main_vbox), node_type_frame, TRUE, TRUE, 0);
			gtk_container_set_border_width (GTK_CONTAINER (node_type_frame), BORDER_WIDTH);

			list_alignment = gtk_alignment_new (0.5, 0.5, 0.4, 0.4);
			gtk_widget_show (list_alignment);
			gtk_container_add (GTK_CONTAINER (node_type_frame), list_alignment);

			if (network_data.devices != 1)
			{
				node_type_list = gtk_clist_new_with_titles(1, node_type_title);
				gtk_clist_append(GTK_CLIST(node_type_list), node_type_alpha);
				gtk_clist_append(GTK_CLIST(node_type_list), node_type_beta);
				gtk_widget_show (node_type_list);
				gtk_container_add (GTK_CONTAINER (list_alignment), node_type_list);

				button_alignment = gtk_alignment_new (0.5, 0.5, 0.3, 1);
				gtk_widget_show (button_alignment);
				gtk_box_pack_start (GTK_BOX (main_vbox), button_alignment, FALSE, FALSE, 0);

				next = gtk_button_new_with_label ("Next");
				gtk_widget_set_sensitive(next, FALSE);
				gtk_widget_show (next);
				gtk_container_add (GTK_CONTAINER (button_alignment), next);

				/* Setup a callback function for when they select */
				setup_info.node_type = -1;
				selected.row_selected = &setup_info.node_type;
				selected.next = next;
				gtk_signal_connect(GTK_OBJECT(node_type_list), "select_row",
					   	   GTK_SIGNAL_FUNC(selection_made), &selected);

				/* Setup the next button so we go to the next window */
				*(int *)data = GET_TZINFO;
				gtk_signal_connect(GTK_OBJECT(next), "clicked", GTK_SIGNAL_FUNC(setup_window), data);
				break;
			}
			else
			{
				/* Set the node type and drive on through */
				setup_info.node_type = 0;
			}
		case GET_TZINFO:
			sprintf(str, "Node type %d\n", setup_info.node_type);
			debug_output(str);

			if (network_data.devices != 1)
			{
				/* Destroy old list */
				gtk_widget_destroy(node_type_list);
				gtk_widget_destroy(next);
			}

			/* Change the frame label */
			gtk_frame_set_label(GTK_FRAME(node_type_frame), "Timezone Selection");
			/* Make our timezone list */
			timezone_select_list = gtk_clist_new_with_titles(1, timezone_title);
			/* Put our data in the rows */
			for (timezone_selected = 0; timezone_selected < TZROWS;
			     timezone_selected++)
				gtk_clist_append(GTK_CLIST(timezone_select_list), timezones[timezone_selected]);
			gtk_widget_show(timezone_select_list);
			gtk_container_add(GTK_CONTAINER(list_alignment), timezone_select_list);

			/* Make a Greyed out next button */
			next = gtk_button_new_with_label("Next");
			gtk_widget_set_sensitive(next, FALSE);
			gtk_widget_show(next);
			gtk_container_add (GTK_CONTAINER (button_alignment), next);

			/* Setup a callback function for when they select timezones */
			timezone_selected = -1;
			selected.row_selected = &timezone_selected;
			selected.next = next;
			gtk_signal_connect(GTK_OBJECT(timezone_select_list), "select_row",
					   GTK_SIGNAL_FUNC(selection_made), &selected);

			/* Setup the next button so we go to the next window */
			if (setup_info.node_type)
				*(int *)data = SYNC_PROMPT;
			else
				*(int *)data = GET_PASSWORD;
			gtk_signal_connect(GTK_OBJECT(next), "clicked", GTK_SIGNAL_FUNC(setup_window), data);
		break;
		case GET_PASSWORD:
			/* Destroy the old stuff */
			gtk_widget_destroy(timezone_select_list);
			gtk_widget_destroy(next);

			/* Heavily adapted from glade */

			/* Setup the alignment for this window */
			gtk_alignment_set(GTK_ALIGNMENT(list_alignment), 0.5, 0.5, 0.1, 0.1);

			password_vbox = gtk_vbox_new (FALSE, 10);
			gtk_widget_show (password_vbox);
			gtk_container_add (GTK_CONTAINER (list_alignment), password_vbox);

			password_prompt_label = gtk_label_new ("Please enter the Administrator Password:");
			gtk_widget_show (password_prompt_label);
			gtk_box_pack_start (GTK_BOX (password_vbox), password_prompt_label, FALSE, FALSE, 0);

			password_table = gtk_table_new (2, 2, FALSE);
			gtk_widget_show (password_table);
			gtk_box_pack_start (GTK_BOX (password_vbox), password_table, FALSE, TRUE, 0);
			gtk_table_set_row_spacings (GTK_TABLE (password_table), BORDER_WIDTH);
			gtk_table_set_col_spacings (GTK_TABLE (password_table), BORDER_WIDTH);

			password1_label = gtk_label_new ("Password:");
			gtk_widget_show (password1_label);
			gtk_table_attach (GTK_TABLE (password_table), password1_label, 0, 1, 0, 1,
				  	  (GtkAttachOptions) (GTK_FILL),
				  	  (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (password1_label), 1, 0.5);

			password2_label = gtk_label_new ("Re-enter Password:");
			gtk_widget_show (password2_label);
			gtk_table_attach (GTK_TABLE (password_table), password2_label, 0, 1, 1, 2,
				  	  (GtkAttachOptions) (GTK_FILL),
				  	  (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (password2_label), 1, 1);

			password1_entry = gtk_entry_new ();
			gtk_entry_set_visibility(GTK_ENTRY(password1_entry), FALSE);
			gtk_widget_show (password1_entry);
			gtk_table_attach (GTK_TABLE (password_table), password1_entry, 1, 2, 0, 1,
				  	  (GtkAttachOptions) (GTK_FILL),
				  	  (GtkAttachOptions) (0), 0, 0);

			password2_entry = gtk_entry_new ();
			gtk_entry_set_visibility(GTK_ENTRY(password2_entry), FALSE);
			gtk_widget_show (password2_entry);
			gtk_table_attach (GTK_TABLE (password_table), password2_entry, 1, 2, 1, 2,
				  	  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
				  	  (GtkAttachOptions) (0), 0, 0);

			/* Change the frame label */
			gtk_frame_set_label(GTK_FRAME(node_type_frame), "Password Configuration");

			/* Setup a button */
			next = gtk_button_new_with_label("Next");
			gtk_widget_show(next);

			/* Add the button to the alignment */
			gtk_container_add(GTK_CONTAINER(button_alignment), next);

			/* Make a callback to check the password */
			pwdata.entry1 = (GtkEntry *)password1_entry;
			pwdata.entry2 = (GtkEntry *)password2_entry;
			gtk_signal_connect(GTK_OBJECT(next), "clicked", GTK_SIGNAL_FUNC(check_passwords), &pwdata);

		break;
		case SYNC_PROMPT:
			gtk_widget_destroy(next);
			gtk_widget_destroy(timezone_select_list);
			gtk_widget_destroy(list_alignment);

			gtk_frame_set_label(GTK_FRAME(node_type_frame), "Network Sync");

			sync_label = gtk_label_new(SYNC_PROMPT_TEXT);
			gtk_widget_show(sync_label);
			gtk_container_add(GTK_CONTAINER(node_type_frame), sync_label);

			next = gtk_button_new_with_label("Next");
			gtk_widget_show(next);
			gtk_container_add(GTK_CONTAINER(button_alignment), next);
		
			*(int *)data = SYNC_WITH_ALPHA;
			gtk_signal_connect(GTK_OBJECT(next), "clicked", GTK_SIGNAL_FUNC(setup_window), data);
		break;
		case SYNC_WITH_ALPHA:
			/* Destroy old stuff */
			gtk_widget_destroy(next);
			gtk_widget_destroy(button_alignment);
			gtk_widget_destroy(sync_label);
			gtk_widget_destroy(node_type_frame);
			gtk_widget_destroy(header_pixmap);

			/* Make the big pixmap with text thing */
			pixmap_fixed = gtk_fixed_new();
			/* Make the background black (labels get their background from the parent) */
			rc_style = gtk_rc_style_new();
			rc_style->bg[GTK_STATE_NORMAL] = black;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_BG;
			gtk_widget_modify_style(pixmap_fixed, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show (pixmap_fixed);
			gtk_box_pack_start (GTK_BOX (main_vbox), pixmap_fixed, TRUE, FALSE, 0);
			gtk_widget_set_usize (pixmap_fixed, MAIN_PIXMAP_WIDTH, MAIN_PIXMAP_HEIGHT);

			image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, BACKGROUND_IMAGE);
			main_pixmap = gtk_pixmap_new(image_data, mask);
			gtk_widget_show(main_pixmap);
			gtk_fixed_put(GTK_FIXED(pixmap_fixed), main_pixmap, 0, 0);
			gtk_widget_set_uposition(main_pixmap, 0, 0);
			gtk_misc_set_alignment(GTK_MISC(main_pixmap), 0, 0);

			pixmap_description = gtk_label_new(SYNC_TEXT);
			/* Make the label grey */
			rc_style = gtk_rc_style_new();
			rc_style->fg[GTK_STATE_NORMAL] = grey;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_FG;
			free(rc_style->font_name);
			rc_style->font_name = malloc(strlen(font_name));
			strcpy(rc_style->font_name, font_name);
			gtk_widget_modify_style(pixmap_description, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show (pixmap_description);
			gtk_fixed_put (GTK_FIXED (pixmap_fixed), pixmap_description, 22, 270);
			gtk_widget_set_uposition (pixmap_description, 22, 270);
			gtk_widget_set_usize (pixmap_description, 0, 0);
			gtk_label_set_justify (GTK_LABEL (pixmap_description), GTK_JUSTIFY_LEFT);
			gtk_misc_set_alignment (GTK_MISC (pixmap_description), 0, 0);

			/* Setup the minor label */
			minor_label = gtk_label_new("Syncing with Alpha");
			gtk_label_set_justify(GTK_LABEL(minor_label), GTK_JUSTIFY_LEFT);
			gtk_misc_set_alignment(GTK_MISC(minor_label), 0, 0);

			/* Make sure the font is smaller */
			rc_style = gtk_rc_style_new();
			free(rc_style->font_name);
			rc_style->font_name = malloc(strlen(minor_font_name));
			strcpy(rc_style->font_name, minor_font_name);
			gtk_widget_modify_style(minor_label, rc_style);
			gtk_widget_show(minor_label);

			/* Stick it in the box */
			gtk_box_pack_start(GTK_BOX(main_vbox), minor_label, FALSE, FALSE, 0);

			/* Create an alignment for our progress bar */
			progress_bar_alignment = gtk_alignment_new (0.5, 0.5, 1, 0.5);
			gtk_widget_show(progress_bar_alignment);
			gtk_box_pack_start(GTK_BOX(main_vbox), progress_bar_alignment, TRUE, TRUE, 0);

			/* Create our progress bar */
			progress_bar = gtk_progress_bar_new();
			gtk_widget_show(progress_bar);
			gtk_container_add(GTK_CONTAINER(progress_bar_alignment), progress_bar);
			gtk_progress_bar_set_activity_step((GtkProgressBar *)progress_bar, PROGBARINC);

			/* Switch to activity display */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, TRUE);
			
			debug_output("Syncing with Alpha\n");

			/* Start a thread */
			sync_struct.progress_bar = (GtkProgress *)progress_bar;
			sync_struct.network_data = &network_data;
			start_thread((void *)sync_with_alpha, (void *)&sync_struct);

			/* Start our update timer */
			update.next_window = NETWORK_CONFIG;
			update.error_window = SYNC_WITH_ALPHA_ERROR;
			update.progress_bar = (GtkProgress *)progress_bar;
			update.minor_label = minor_label;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case SYNC_WITH_ALPHA_ERROR:
			/* throw up error with option to go back to test */
			debug_output("Error syncing with Alpha\n");
			error_msg("Unable to sync with alpha, please make sure cables installed correctly\nClick OK to try again", TRUE);

			/* Now try it again */
			start_thread((void *)sync_with_alpha, (void *)&network_data);
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case NETWORK_CONFIG:
			/* Destroy the old stuff */
			/* NOTE: There the screen before this is different depending on
			 * if your configuring alpha or beta */
			if (setup_info.node_type)
			{
				gtk_widget_destroy(progress_bar);
				gtk_widget_destroy(progress_bar_alignment);
				gtk_widget_destroy(minor_label);
				gtk_widget_destroy(pixmap_description);
				gtk_widget_destroy(main_pixmap);
				gtk_widget_destroy(pixmap_fixed);
				
				/* Make the header, the frame, and the alignment */
				image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, HEADER_IMAGE);
				header_pixmap = gtk_pixmap_new(image_data, mask);
				gtk_widget_show (header_pixmap);
				gtk_box_pack_start (GTK_BOX (main_vbox), header_pixmap, FALSE, FALSE, 0);

				node_type_frame = gtk_frame_new("");
				gtk_widget_show (node_type_frame);
				gtk_box_pack_start (GTK_BOX (main_vbox), node_type_frame, TRUE, TRUE, 0);
				gtk_container_set_border_width (GTK_CONTAINER (node_type_frame), BORDER_WIDTH);

				button_alignment = gtk_alignment_new (0.5, 0.5, 0.3, 1);
				gtk_widget_show (button_alignment);
				gtk_box_pack_start (GTK_BOX (main_vbox), button_alignment, FALSE, FALSE, 0);
			}
			else
			{
				gtk_widget_destroy(next);
				gtk_widget_destroy(password_prompt_label);
				gtk_widget_destroy(password1_label);
				gtk_widget_destroy(password2_label);
				gtk_widget_destroy(password1_entry);
				gtk_widget_destroy(password2_entry);
				gtk_widget_destroy(password_table);
				gtk_widget_destroy(password_vbox);
				gtk_widget_destroy(list_alignment);
			}

			/* This code is adapted from Glade */
			gtk_frame_set_label(GTK_FRAME(node_type_frame), "Network Configuration");

			network_config_hbox = gtk_hbox_new (TRUE, 0);
			gtk_widget_show (network_config_hbox);
			gtk_container_add (GTK_CONTAINER (node_type_frame), network_config_hbox);

			network_config_vbox = gtk_vbox_new (FALSE, 0);
			gtk_widget_show (network_config_vbox);
			gtk_box_pack_start (GTK_BOX (network_config_hbox), network_config_vbox, TRUE, TRUE, 0);

			cluster_frame_alignment = gtk_alignment_new (0.5, 0.5, 1, 0.4);
			gtk_widget_show (cluster_frame_alignment);
			gtk_box_pack_start (GTK_BOX (network_config_vbox), cluster_frame_alignment, FALSE, TRUE, 0);

			cluster_ip_frame = gtk_frame_new ("Cluster");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(cluster_ip_frame, FALSE);
			gtk_widget_show (cluster_ip_frame);
			gtk_container_add (GTK_CONTAINER (cluster_frame_alignment), cluster_ip_frame);
			gtk_container_set_border_width (GTK_CONTAINER (cluster_ip_frame), BORDER_WIDTH);

			cluster_frame_vbox = gtk_vbox_new (FALSE, 0);
			gtk_widget_show (cluster_frame_vbox);
			gtk_container_add (GTK_CONTAINER (cluster_ip_frame), cluster_frame_vbox);
			gtk_container_set_border_width (GTK_CONTAINER (cluster_frame_vbox), 5);

			cluster_ip_hbox = gtk_hbox_new (FALSE, BORDER_WIDTH);
			gtk_widget_show (cluster_ip_hbox);
			gtk_box_pack_start (GTK_BOX (cluster_frame_vbox), cluster_ip_hbox, FALSE, FALSE, 0);

			cluster_ip_label = gtk_label_new ("IP Address");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(cluster_ip_label, FALSE);
			gtk_widget_show (cluster_ip_label);
			gtk_box_pack_start (GTK_BOX (cluster_ip_hbox), cluster_ip_label, FALSE, TRUE, 0);
			gtk_misc_set_alignment (GTK_MISC (cluster_ip_label), 0.5, 1);

			cluster_ip_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(cluster_ip_entry), network_data.cluster_ip);
				gtk_widget_set_sensitive(cluster_ip_entry, FALSE);
			}
			gtk_widget_show (cluster_ip_entry);
			gtk_box_pack_start (GTK_BOX (cluster_ip_hbox), cluster_ip_entry, FALSE, TRUE, 0);
			gtk_widget_set_usize(cluster_ip_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			node_hbox = gtk_hbox_new (TRUE, BORDER_WIDTH);
			gtk_widget_show (node_hbox);
			gtk_box_pack_start (GTK_BOX (network_config_vbox), node_hbox, TRUE, TRUE, 0);
			gtk_container_set_border_width (GTK_CONTAINER (node_hbox), BORDER_WIDTH);

			if (setup_info.node_type)
			{
				primary_frame = gtk_frame_new("Other Computer");
				gtk_widget_set_sensitive(primary_frame, FALSE);
			}
			else
				primary_frame = gtk_frame_new ("This Computer");
			gtk_widget_show (primary_frame);
			gtk_box_pack_start (GTK_BOX (node_hbox), primary_frame, TRUE, TRUE, 0);

			primary_table = gtk_table_new (6, 2, FALSE);
			gtk_widget_show (primary_table);
			gtk_container_add (GTK_CONTAINER (primary_frame), primary_table);
			gtk_table_set_row_spacings (GTK_TABLE (primary_table), BORDER_WIDTH);

			primary_ip_label = gtk_label_new ("IP Address");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(primary_ip_label, FALSE);
			gtk_widget_show (primary_ip_label);
			gtk_table_attach (GTK_TABLE (primary_table), primary_ip_label, 0, 1, 1, 2,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (primary_ip_label), 1, 0.5);

			primary_hostname_label = gtk_label_new ("Hostname");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(primary_hostname_label, FALSE);
			gtk_widget_show (primary_hostname_label);
			gtk_table_attach (GTK_TABLE (primary_table), primary_hostname_label, 0, 1, 0, 1,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (primary_hostname_label), 1, 0.5);

			primary_hostname_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(primary_hostname_entry), network_data.primary_hostname);
				gtk_widget_set_sensitive(primary_hostname_entry, FALSE);
			}
			gtk_widget_show (primary_hostname_entry);
			gtk_table_attach (GTK_TABLE (primary_table), primary_hostname_entry, 1, 2, 0, 1,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (primary_hostname_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			primary_subnet_label = gtk_label_new ("Subnet");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(primary_subnet_label, FALSE);
			gtk_widget_show (primary_subnet_label);
			gtk_table_attach (GTK_TABLE (primary_table), primary_subnet_label, 0, 1, 2, 3,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (primary_subnet_label), 1, 0.5);

			primary_subnet_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(primary_subnet_entry), network_data.primary_subnet);
				gtk_widget_set_sensitive(primary_subnet_entry, FALSE);
			}
			gtk_widget_show (primary_subnet_entry);
			gtk_table_attach (GTK_TABLE (primary_table), primary_subnet_entry, 1, 2, 2, 3,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (primary_subnet_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			primary_ip_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(primary_ip_entry), network_data.primary_ip);
				gtk_widget_set_sensitive(primary_ip_entry, FALSE);
			}
			gtk_widget_show (primary_ip_entry);
			gtk_table_attach (GTK_TABLE (primary_table), primary_ip_entry, 1, 2, 1, 2,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (primary_ip_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			primary_gateway_label = gtk_label_new ("Gateway");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(primary_gateway_label, FALSE);
			gtk_widget_show (primary_gateway_label);
			gtk_table_attach (GTK_TABLE (primary_table), primary_gateway_label, 0, 1, 3, 4,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (primary_gateway_label), 1, 0.5);

			primary_gateway_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(primary_gateway_entry), network_data.primary_gateway);
				gtk_widget_set_sensitive(primary_gateway_entry, FALSE);
			}
			gtk_widget_show (primary_gateway_entry);
			gtk_table_attach (GTK_TABLE (primary_table), primary_gateway_entry, 1, 2, 3, 4,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (primary_gateway_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			primary_dns1_label = gtk_label_new ("Primary DNS");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(primary_dns1_label, FALSE);
			gtk_widget_show (primary_dns1_label);
			gtk_table_attach (GTK_TABLE (primary_table), primary_dns1_label, 0, 1, 4, 5,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (primary_dns1_label), 1, 0.5);

			primary_dns1_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(primary_dns1_entry), network_data.primary_dns1);
				gtk_widget_set_sensitive(primary_dns1_entry, FALSE);
			}
			gtk_widget_show (primary_dns1_entry);
			gtk_table_attach (GTK_TABLE (primary_table), primary_dns1_entry, 1, 2, 4, 5,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (primary_dns1_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			primary_dns2_label = gtk_label_new ("Secondary DNS");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(primary_dns2_label, FALSE);
			gtk_widget_show (primary_dns2_label);
			gtk_table_attach (GTK_TABLE (primary_table), primary_dns2_label, 0, 1, 5, 6,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (primary_dns2_label), 1, 0.5);

			primary_dns2_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(primary_dns2_entry), network_data.primary_dns2);
				gtk_widget_set_sensitive(primary_dns2_entry, FALSE);
			}
			gtk_widget_show (primary_dns2_entry);
			gtk_table_attach (GTK_TABLE (primary_table), primary_dns2_entry, 1, 2, 5, 6,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (primary_dns2_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			if (setup_info.node_type)
				secondary_frame = gtk_frame_new("This Computer");
			else
				secondary_frame = gtk_frame_new("Other Computer");
			gtk_widget_show (secondary_frame);
			gtk_box_pack_start (GTK_BOX (node_hbox), secondary_frame, TRUE, TRUE, 0);

			secondary_table = gtk_table_new (6, 2, FALSE);
			gtk_widget_show (secondary_table);
			gtk_container_add (GTK_CONTAINER (secondary_frame), secondary_table);
			gtk_table_set_row_spacings (GTK_TABLE (secondary_table), BORDER_WIDTH);

			secondary_hostname_label = gtk_label_new ("Hostname");
			if (setup_info.node_type)
				gtk_widget_set_sensitive(secondary_hostname_label, FALSE);
			gtk_widget_show (secondary_hostname_label);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_hostname_label, 0, 1, 0, 1,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (secondary_hostname_label), 1, 0.5);

			secondary_ip_label = gtk_label_new ("IP Address");
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_ip_label, FALSE);
			gtk_widget_show (secondary_ip_label);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_ip_label, 0, 1, 1, 2,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (secondary_ip_label), 1, 0.5);

			secondary_subnet_label = gtk_label_new ("Subnet");
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_subnet_label, FALSE);
			gtk_widget_show (secondary_subnet_label);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_subnet_label, 0, 1, 2, 3,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (secondary_subnet_label), 1, 0.5);

			secondary_hostname_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (setup_info.node_type)
			{
				gtk_entry_set_text(GTK_ENTRY(secondary_hostname_entry), network_data.secondary_hostname);
				gtk_widget_set_sensitive(secondary_hostname_entry, FALSE);
			}
			gtk_widget_show (secondary_hostname_entry);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_hostname_entry, 1, 2, 0, 1,
				   (GtkAttachOptions) (0),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (secondary_hostname_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			secondary_ip_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_ip_entry, FALSE);
			gtk_widget_show (secondary_ip_entry);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_ip_entry, 1, 2, 1, 2,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (secondary_ip_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			secondary_subnet_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_subnet_entry, FALSE);
			gtk_widget_show (secondary_subnet_entry);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_subnet_entry, 1, 2, 2, 3,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (secondary_subnet_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			secondary_gateway_label = gtk_label_new ("Gateway");
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_gateway_label, FALSE);
			gtk_widget_show (secondary_gateway_label);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_gateway_label, 0, 1, 3, 4,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (secondary_gateway_label), 1, 0.5);

			secondary_gateway_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_gateway_entry, FALSE);
			gtk_widget_show (secondary_gateway_entry);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_gateway_entry, 1, 2, 3, 4,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (secondary_gateway_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			secondary_dns1_label = gtk_label_new ("Primary DNS");
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_dns1_label, FALSE);
			gtk_widget_show (secondary_dns1_label);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_dns1_label, 0, 1, 4, 5,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (secondary_dns1_label), 1, 0.5);

			secondary_dns1_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_dns1_entry, FALSE);
			gtk_widget_show (secondary_dns1_entry);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_dns1_entry, 1, 2, 4, 5,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (secondary_dns1_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			secondary_dns2_label = gtk_label_new ("Secondary DNS");
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_dns2_label, FALSE);
			gtk_widget_show (secondary_dns2_label);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_dns2_label, 0, 1, 5, 6,
				   (GtkAttachOptions) (GTK_FILL),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_misc_set_alignment (GTK_MISC (secondary_dns2_label), 1, 0.5);

			secondary_dns2_entry = gtk_entry_new_with_max_length (IP_ADDRESS_CHARS);
			if (!setup_info.node_type)
				gtk_widget_set_sensitive(secondary_dns2_entry, FALSE);
			gtk_widget_show (secondary_dns2_entry);
			gtk_table_attach (GTK_TABLE (secondary_table), secondary_dns2_entry, 1, 2, 5, 6,
				   (GtkAttachOptions) (GTK_EXPAND),
				   (GtkAttachOptions) (0), 0, 0);
			gtk_widget_set_usize (secondary_dns2_entry, IP_ADDRESS_ENTRY_SIZE, -2);

			/* Connect the next button to our checking routine */
			next = gtk_button_new_with_label ("Next");
			gtk_widget_show (next);
			gtk_container_add (GTK_CONTAINER (button_alignment), next);

			/* Setup the data we need to check */
			network_data.cluster_ip_entry = cluster_ip_entry;
			network_data.secondary_hostname_entry = secondary_hostname_entry;
			network_data.primary_hostname_entry = primary_hostname_entry;
			if (setup_info.node_type)
			{
				network_data.ip_entry = secondary_ip_entry;
				network_data.dns1_entry = secondary_dns1_entry;
				network_data.dns2_entry = secondary_dns2_entry;
				network_data.gateway_entry = secondary_gateway_entry;
				network_data.subnet_entry = secondary_subnet_entry;
			}
			else
			{
				network_data.ip_entry = primary_ip_entry;
				network_data.dns1_entry = primary_dns1_entry;
				network_data.dns2_entry = primary_dns2_entry;
				network_data.gateway_entry = primary_gateway_entry;
				network_data.subnet_entry = primary_subnet_entry;
			}
			network_data.current_section = CHECK_ENTRIES;
			gtk_signal_connect(GTK_OBJECT(next), "clicked", GTK_SIGNAL_FUNC(check_network_config), &network_data);
		break;
		case SETUP_CONFIGS:
			/* Destroy old stuff */
			gtk_widget_destroy(next);
			gtk_widget_destroy(button_alignment);
			gtk_widget_destroy(primary_ip_label);
			gtk_widget_destroy(primary_ip_entry);
			gtk_widget_destroy(primary_hostname_label);
			gtk_widget_destroy(primary_hostname_entry);
			gtk_widget_destroy(primary_gateway_label);
			gtk_widget_destroy(primary_gateway_entry);
			gtk_widget_destroy(primary_subnet_label);
			gtk_widget_destroy(primary_subnet_entry);
			gtk_widget_destroy(primary_dns1_label);
			gtk_widget_destroy(primary_dns1_entry);
			gtk_widget_destroy(primary_dns2_label);
			gtk_widget_destroy(primary_dns2_entry);
			gtk_widget_destroy(primary_table);
			gtk_widget_destroy(primary_frame);
			gtk_widget_destroy(secondary_ip_label);
			gtk_widget_destroy(secondary_ip_entry);
			gtk_widget_destroy(secondary_hostname_label);
			gtk_widget_destroy(secondary_hostname_entry);
			gtk_widget_destroy(secondary_gateway_label);
			gtk_widget_destroy(secondary_gateway_entry);
			gtk_widget_destroy(secondary_subnet_label);
			gtk_widget_destroy(secondary_subnet_entry);
			gtk_widget_destroy(secondary_dns1_label);
			gtk_widget_destroy(secondary_dns1_entry);
			gtk_widget_destroy(secondary_dns2_label);
			gtk_widget_destroy(secondary_dns2_entry);
			gtk_widget_destroy(secondary_table);
			gtk_widget_destroy(secondary_frame);
			gtk_widget_destroy(node_hbox);
			gtk_widget_destroy(cluster_ip_label);
			gtk_widget_destroy(cluster_ip_entry);
			gtk_widget_destroy(cluster_ip_hbox);
			gtk_widget_destroy(cluster_frame_vbox);
			gtk_widget_destroy(cluster_ip_frame);
			gtk_widget_destroy(cluster_frame_alignment);
			gtk_widget_destroy(network_config_vbox);
			gtk_widget_destroy(network_config_hbox);
			gtk_widget_destroy(node_type_frame);
			gtk_widget_destroy(header_pixmap);

			pixmap_fixed = gtk_fixed_new ();
			/* Make the background black (labels get their background from the parent) */
			rc_style = gtk_rc_style_new();
			rc_style->bg[GTK_STATE_NORMAL] = black;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_BG;
			gtk_widget_modify_style(pixmap_fixed, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show (pixmap_fixed);
			gtk_box_pack_start (GTK_BOX (main_vbox), pixmap_fixed, TRUE, FALSE, 0);
			gtk_widget_set_usize (pixmap_fixed, MAIN_PIXMAP_WIDTH, MAIN_PIXMAP_HEIGHT);

			image_data = gdk_pixmap_create_from_xpm(NULL, &mask, NULL, BACKGROUND_IMAGE);
			main_pixmap = gtk_pixmap_new(image_data, mask);
			gtk_widget_show (main_pixmap);
			gtk_fixed_put (GTK_FIXED (pixmap_fixed), main_pixmap, 0, 0);
			gtk_widget_set_uposition (main_pixmap, 0, 0);
			gtk_misc_set_alignment (GTK_MISC (main_pixmap), 0, 0);

			pixmap_description = gtk_label_new (CLEANINGUP_TEXT);
			/* Make the label grey */
			rc_style = gtk_rc_style_new();
			rc_style->fg[GTK_STATE_NORMAL] = grey;
			rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_FG;
			free(rc_style->font_name);
			rc_style->font_name = malloc(strlen(font_name));
			strcpy(rc_style->font_name, font_name);
			gtk_widget_modify_style(pixmap_description, rc_style);
			gtk_rc_style_unref(rc_style);
			gtk_widget_show (pixmap_description);
			gtk_fixed_put (GTK_FIXED (pixmap_fixed), pixmap_description, 22, 270);
			gtk_widget_set_uposition (pixmap_description, 22, 270);
			gtk_widget_set_usize (pixmap_description, 0, 0);
			gtk_label_set_justify (GTK_LABEL (pixmap_description), GTK_JUSTIFY_LEFT);
			gtk_misc_set_alignment (GTK_MISC (pixmap_description), 0, 0);

			/* Setup the minor label */
			minor_label = gtk_label_new("Creating config files");
			gtk_label_set_justify(GTK_LABEL(minor_label), GTK_JUSTIFY_LEFT);
			gtk_misc_set_alignment(GTK_MISC(minor_label), 0, 0);

			/* Make sure the font is smaller */
			rc_style = gtk_rc_style_new();
			free(rc_style->font_name);
			rc_style->font_name = malloc(strlen(minor_font_name));
			strcpy(rc_style->font_name, minor_font_name);
			gtk_widget_modify_style(minor_label, rc_style);
			gtk_widget_show(minor_label);

			/* Stick it in the box */
			gtk_box_pack_start(GTK_BOX(main_vbox), minor_label, FALSE, FALSE, 0);

			/* Create an alignment for our progress bar */
			progress_bar_alignment = gtk_alignment_new (0.5, 0.5, 1, 0.5);
			gtk_widget_show(progress_bar_alignment);
			gtk_box_pack_start(GTK_BOX(main_vbox), progress_bar_alignment, TRUE, TRUE, 0);

			/* Create our progress bar */
			progress_bar = gtk_progress_bar_new();
			gtk_widget_show(progress_bar);
			gtk_container_add(GTK_CONTAINER(progress_bar_alignment), progress_bar);

			/* Switch to activity display */
			gtk_progress_set_activity_mode((GtkProgress *)progress_bar, TRUE);

			debug_output("Setting up config files\n");

			/* Start a thread */
			setup_info.mount_data = &mount_data;
			setup_info.timezone = timezones[0][timezone_selected];
			setup_info.network_data = &network_data;
			setup_info.password = pwdata.password;
			start_thread((void *)setup_configs, (void *)&setup_info);

			/* Start our update timer */
			update.next_window = KERNEL_INSTALL;
			update.error_window = SETUP_CONFIGS_ERROR;
			update.progress_bar = (GtkProgress *)progress_bar;
			update.minor_label = minor_label;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case SETUP_CONFIGS_ERROR:
			/*--ERROR--*/
			debug_output("Error while setting up config files\n");
		break;
		case KERNEL_INSTALL:
			gtk_label_set_text(GTK_LABEL(minor_label), "Installing kernel");
			debug_output("Installing kernel\n");

			/* Start a thread */
			start_thread((void *)setup_kernel, NULL);

			/* Start our update timer */
			if (setup_info.node_type)
				update.next_window = FINAL_SYNC;
			else
				update.next_window = UNMOUNT;
			update.error_window = KERNEL_INSTALL_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case KERNEL_INSTALL_ERROR:
			/*--ERROR--*/
			debug_output("Error while installing kernel\n");
		break;
		case FINAL_SYNC:
			minor_label_text = NULL;
			gtk_label_set_text(GTK_LABEL(minor_label), "Performing final sync");
			debug_output("Performing final sync\n");

			/* Start a thread */
			start_thread((void *)final_sync, (void *)&network_data);

			/* Start our update timer */
			update.next_window = UNMOUNT;
			update.error_window = FINAL_SYNC_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case FINAL_SYNC_ERROR:
			debug_output("Error while performing final sync\n");
			error_msg(FINAL_SYNC_ERROR_TEXT, FALSE);
		break;
		case UNMOUNT:
			gtk_label_set_text(GTK_LABEL(minor_label), "Unmounting partitions");
			debug_output("Unmounting\n");

			/* Start a thread */
			start_thread((void *)unmount_partitions, (void *)&mount_data);

			/* Start our update timer */
			update.next_window = REBOOT;
			update.error_window = UNMOUNT_ERROR;
			gtk_timeout_add(UPDATE_TIME, (GtkFunction)update_progress_bar, &update);
		break;
		case UNMOUNT_ERROR:
			debug_output("Error while unmounting\n");
			error_msg(UNMOUNT_ERROR_TEXT, FALSE);
		break;
		case REBOOT:
			gtk_widget_destroy(progress_bar);

			gtk_label_set(GTK_LABEL(pixmap_description), PLEASEREBOOT_TEXT);
			gtk_label_set(GTK_LABEL(minor_label), "");
		break;
	}
}

int main(int argc,
	 char *argv[])
{
	static int data;

	gtk_init(&argc, &argv);

	/* Start at the beginning */
	data = INTRO_WIN;
	setup_window(NULL, &data);

	gtk_main();

	return(0);
}
