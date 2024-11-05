#ifndef _GTK_INSTALL_H
#define _GTK_INSTALL_H

#include <gtk/gtk.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define __USE_XOPEN /* to get prototype for crypt */
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/swap.h>
#include <sys/mount.h>

#define DEBUG_LOG "/tmp/gtk_install.log"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAIN_PIXMAP_WIDTH 780
#define MAIN_PIXMAP_HEIGHT 400
#define IP_ADDRESS_CHARS 15
#define IP_ADDRESS_ENTRY_SIZE 180
#define BORDER_WIDTH 10
#define BOTTOM_BUTTON_H_PERCENT 0.50
#define BOTTOM_BUTTON_V_PERCENT 0.10

/* Constants for window identification */
#define INTRO_WIN 0
#define CHECK_DRIVE_WIN 1
#define BAD_BLOCKS_ERROR_WIN 2
#define CREATE_PARTITIONS 3 
#define CREATE_PARTITIONS_ERROR 4
#define MAKE_SWAP 5
#define MAKE_SWAP_ERROR 6
#define MAKE_SNAPPTIX 7
#define MAKE_SNAPPTIX_ERROR 8
#define MAKE_USER 9
#define MAKE_USER_ERROR 10
#define MOUNT_PARTITIONS 11
#define MOUNT_PARTITIONS_ERROR 12
#define INSTALL_PACKAGES 13
#define INSTALL_PACKAGES_ERROR 14
#define SETUP_CONFIGS 15
#define SETUP_CONFIGS_ERROR 16
#define KERNEL_INSTALL 17
#define KERNEL_INSTALL_ERROR 18
#define GET_TZINFO 19
#define NETWORK_DETECT 21
#define NETWORK_DETECT_ERROR 22
#define INSTALL_NICS 23
#define INSTALL_NICS_ERROR 24
#define GET_PASSWORD 25
#define NODE_TYPE_SELECT 26
#define ADAPTER_SETUP 27
#define REBOOT	28
#define NETWORK_CONFIG 29
#define SYNC_WITH_ALPHA 30
#define SYNC_WITH_ALPHA_ERROR 31
#define SYNC_PROMPT 32
#define FINAL_SYNC 33
#define FINAL_SYNC_ERROR 34
#define CHECK_NET 35
#define UNMOUNT 36
#define UNMOUNT_ERROR 37
#define CONTINUE_CHECK 38
#define QUERY_ERROR 39
#define QUERY_REPAIR 40

#define BACKGROUND_IMAGE "/snapptix/images/background.xpm"
#define HEADER_IMAGE "/snapptix/images/header.xpm"
#define GREY_VALUE 39064 /* The value of our grey text color */

/* This is the text displayed throughout the install */
#define WELCOME_TEXT "Welcome!\nThis installer will load SnappCluster onto your system.\nClick the \"next\" button to continue."
#define HDCHECK_TEXT "Please wait while we check the hard drive for stability.\nThis can take up to 20 minutes."
#define INSTALLING_TEXT "Installing SnappCluster..."
#define NETDETECT_TEXT "Checking network adapters..."
#define SYNC_TEXT "Gathering information from other node..."
#define SYNC_PROMPT_TEXT "Please disconnect all networking cables from the back of\nthe computer, and connect the private crossover cable to\nthe other node"
#define CLEANINGUP_TEXT "Cleaning up..."
#define PLEASEREBOOT_TEXT "SnappCluster is finished installing.\nPlease reboot your computer and eject the CD."
#define BAD_BLOCKS_ERROR_TEXT "Badblocks found on hard drive"
#define HARDDRIVE_ERROR_TEXT "Unable to find hard drive"
#define NETDETECT_ERROR_TEXT "Network detection error"
#define MAKE_USER_ERROR_TEXT "Error while creating user partition"
#define PREVIOUS_INSTALL_TEXT "A previous install was detected.  Click yes to keep your user data\nand reinstall the SNAPPTIX data, click no to reinstall them both.\nNote if this is a Beta install your user data will resync witht the\nAlpha no matter what."
#define UNMOUNT_ERROR_TEXT "Error while unmounting partitions"
#define CORRUPT_PARTITION_TEXT "A previous install was detected, but your partition table is corrupt.\nRecovery is impossible, click next to reinstall."
#define FINAL_SYNC_ERROR_TEXT "Error during final syncronization"
#define NONETDEVICE_ERROR_TEXT "No network adapters found"

/* net check script */
#define NET_TEST_SCRIPT "/snapptix/test_net_params.sh"

/* Timer value for updates (ms) */
#define UPDATE_TIME 500

/* Progress bar increment for activity mode */
#define PROGBARINC 4

#define BUFFSIZE 256

/* Return values for read_until_token and read_off_whitespace */
#define READ_ERR 2
#define READ_DONE 1
#define READ_OK 0

/* Hex IDs for partitions */ 
#define SWAP_ID "0x82"
#define LINUX_ID "0x83"

/* Size of system partition (in bytes) */
#define SNAPPTIX_SIZE 300000000

/* Badblocks file */
#define BADBLOCKS_FILE "/tmp/badblocks"

/* Temporary shell script for setting up partitions */
#define PARTITION_SH "/tmp/setup_partitions.sh"

/* Backup file for partitions */
#define PARTITION_BAK "/tmp/partitions.bak"

#define BACKSPACE 0x08

/* Thread defines */
#define THREAD_EXIT(retptr) { set_finished();\
			    return retptr; }

/* Setup Defines */
#define KERNEL_FILE "/boot/vmlinuz"
#define INSTALL_KERNEL_FILE "/mnt/vmlinuz"

/* Network options for our cluster link */
#define LOCAL_NET_ALPHA_IP "192.168.217.1"
#define LOCAL_NET_BETA_IP "192.168.217.2"
#define LOCAL_NET_SUBNET "255.255.255.0"
#define LOCAL_NET_BROADCAST "192.168.217.255"

/* Globals (mostly for thread communication) */
pthread_t 	tid;
pthread_mutex_t mut;
gboolean 	thread_finished;
gfloat 		percent_done;
extern char *minor_label_text;

/* These are flags for the network check routine */
#define CHECK_ENTRIES 0
#define CHECK_EXIT 1

/* struct defines for our sync_with_alpha function */
struct sync_with_alpha_struct {
	struct network_info *network_data;
	GtkProgress *progress_bar;
};

/* Struct defines for our find_connected_external function */
struct find_connected_struct {
	char *ip;
	char *gateway;
	char *netmask;
	char *dns1;
	char *dns2;
	char *broadcast;
	int check_devices;
	int *detected_device;
};

/* Struct defines for our check_network_config function */
struct network_info {
	GtkWidget *ip_entry;
	GtkWidget *subnet_entry;
	GtkWidget *cluster_ip_entry;
	GtkWidget *primary_hostname_entry;
	GtkWidget *secondary_hostname_entry;
	GtkWidget *dns1_entry;
	GtkWidget *dns2_entry;
	GtkWidget *gateway_entry;
	char ip[BUFFSIZE];
	char gateway[BUFFSIZE];
	char subnet[BUFFSIZE];
	char primary_hostname[BUFFSIZE];
	char secondary_hostname[BUFFSIZE];
	char private_device[BUFFSIZE];
	char public_device[BUFFSIZE];
	char dns1[BUFFSIZE];
	char dns2[BUFFSIZE];
	char cluster_ip[BUFFSIZE];
	char primary_ip[BUFFSIZE];
	char primary_public_device[BUFFSIZE];
	char primary_subnet[BUFFSIZE];
	char primary_gateway[BUFFSIZE];
	char primary_dns1[BUFFSIZE];
	char primary_dns2[BUFFSIZE];
	int current_section;
	char *modules[2];
	unsigned short devices;
};

/* Struct defines for out update_network_config function */
struct update_network_struct {
	struct network_info *network_data;
	GtkProgress *progress_bar;
};

/* Struct defines for passing to our selected function */
struct selected_struct {
	GtkWidget *next;
	int *row_selected;
};

/* Struct defines for passing to our update function */
struct update_struct {
	GtkProgress *progress_bar;
	GtkWidget *minor_label;
	int next_window;
	int error_window;
};

/* Struct defines for passing to our setup_configs function */
struct setup_configs_struct {
	struct mount_info *mount_data;
	struct network_info *network_data;
	int node_type;
	char *timezone;
	char *password;
};

/* Struct defines for passing to our mount_partitions
 * and setup_configs function */
struct mount_info {
	char root_device[BUFFSIZE];
	char snapptix[BUFFSIZE];
	char user[BUFFSIZE];
	char swap[BUFFSIZE];
};

/* Struct defines for passing to our password check
 * callback */
struct pwinfo {
	GtkEntry *entry1;
	GtkEntry *entry2;
	char password[BUFFSIZE];
};

/* hdutils data types */
struct discs {
	char *path;
	struct discs *next;
};

/* gtk_install prototypes */
void setup_window(GtkWidget *, gpointer);
void check_network_config(GtkWidget *, struct network_info *);
void debug_output(char *);

/* hdutils prototypes */
unsigned short 	read_until_token(int, char *, char, unsigned);
unsigned short 	read_off_whitespace(int, char *);
gboolean 	*bad_block_scan(char *);
short 		check_for_user_data(char *);
struct 		discs **find_discs(char *, struct discs **);
char 		*find_snapptix_partition(struct discs *);
gboolean 	*create_partitions(char *);
gboolean	*make_swap(char *);
gboolean	*make_user(char *);
gboolean	*make_snapptix(char *);
gboolean	*mount_partitions(struct mount_info *);
gboolean 	*unmount_partitions();
unsigned long long get_hdsize(char *);

/* threads prototypes */
void set_finished();
void update_thread_info(gfloat, char *);
void start_thread(void *, void *);

/* packages prototypes */
gboolean *install_base(GtkWidget *);

/* configs prototypes */
gboolean *setup_configs(struct setup_configs_struct *);
gboolean *setup_kernel();

/* network prototypes */
gboolean *detect_nic_cards(struct network_info *);
gboolean *install_nic_modules(struct network_info *);
gboolean *sync_with_alpha(struct sync_with_alpha_struct *);
gboolean *final_sync(struct network_info *);
gboolean *find_connected_external(struct find_connected_struct *);

#endif /* _GTK_INSTALL_H */
