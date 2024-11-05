#ifndef __SNAPPTIX__utils_h
#define __SNAPPTIX__utils_h

#include "headers.h"
#include "constants.h"

/* Macros */
#define DEBUGLOGICERROR		printf("Logic error on line %d\n", __LINE__);exit(LOGICERROR)
#define GOT_HERE				printf("Got to line  %d\n", __LINE__)

/* Globals */
static char*	password; 		/* needed to be global for callback function */
FILE*				out; 	/* Allows us to redirect output */
int 				sd;	/* These other three are related to */								
SSL* 				ssl;	/* client socket handling - need to */
SSL_CTX* 		ctx;	/* be able to gracefully close it	*/			

/* Function Prototypes */
void sigcatcher(int);
int criticalFailure(int,char*);
unsigned short readln(int,char*,unsigned);
int closeRequest(int);
static int password_callback(char*,int,int,void*);
int loadSSL(SSL_METHOD*,char*,char*,char*);
int validateClient();
int resetFailoverCount();
int incrementFailoverCount();
int getFailoverCount();
int removeString(char*,int,char*);
void timestamp(char*);
int isPassword(char*);
int changePassword(char*);

/* Functions */  
void sigcatcher(int sig) {
	char buffer[256];
	/* Right now, quit */
	timestamp(buffer);
	fprintf(out, "%s Graceful shutdown.\n", buffer);
   closeRequest(EVERYTHINGOK); /* calls exit(0); */
}

int criticalFailure(int exitCode, char * error) {
	char buffer[256];
	timestamp(buffer);
	fprintf(out, "%s ",buffer);
	fprintf(out, error);
	if (exitCode == SYSTEMCALLFAILURE || exitCode == DELIBERATEREBOOT) {
		fprintf(out, "\nRestarting...\n");
#ifdef RELEASE
		reboot(LINUX_REBOOT_CMD_RESTART); 
#endif
	}
	exit(exitCode);	/* if it gets here, it's pretty bad OR no su */
	return NOEXIT;			/* if it gets here, it's REALLY bad */
}

/* This closes and term‌inates the child process & its SSL engine */
int closeRequest(int exitCode) {
	close(sd);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	fclose(out);	/* Global output variable */
	if (exitCode != NOEXIT) {
		exit(exitCode);
	}
	return NOEXIT;	/* appropriate even if the exit fails */
}

/* Reads up until a newline from a file descriptor and returns a string */
/* Thanks go to Ryan for this function */
unsigned short readln( int fd, char *buff, unsigned max_length) {
	unsigned position = 0;
	int read_bytes;
	do	{
		read_bytes = read(fd, buff + position, 1);
		if (read_bytes < 0) 	return READ_ERR;
		if (!read_bytes) 		return READ_DONE;
		if (buff[position] == '\n') {
			/* make it a string */
			buff[position] = '\0';
			return READ_OK;
		}
		position++;
	} while (position < max_length);
	return READ_ERR;
}

/* Necessary for pre-entering PEM passphrase */
/* Remember: password is a global variable 
	that must be set before calling this 
	callback function.								*/
static int password_callback(char* buf, int length, int rwflags, void* userdata) {
	if (length < strlen(password)+1) 
		return 0;
	/* otherwise */
	strcpy(buf,password);
	return strlen(password);
}

int loadSSL(SSL_METHOD * meth,char* certfile, char* keyfile, char* pswd) {
	ctx = SSL_CTX_new(meth);					
  	if (!ctx) 
		return SSLERROR;
	if (SSL_CTX_use_certificate_file(ctx, certfile, SSL_FILETYPE_PEM) <= 0) 
		return MISSINGCERT;
	
	password = pswd;
	SSL_CTX_set_default_passwd_cb(ctx, password_callback);
	if (SSL_CTX_use_RSAPrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) <= 0) {		/*--CRITICAL FAILURE: RESTORE BACKUP COPY OF PK FILE AND REBOOT--*/ 
		return MISSINGPK;
	}
	if (!SSL_CTX_check_private_key(ctx)) {
		/*--CRITICAL FAILURE: RESTORE BACKUP COPIES OF CERT & PK FILES AND RESTART--*/ 
		return WRONGCERT;	}
	/* otherwise... */
	return EVERYTHINGOK;	
}

int validateClient() {
	char buffer[256];
	char * str;
	X509* client_cert = SSL_get_peer_certificate(ssl);
	if (client_cert == NULL) return 0;	/* need certficate to get in */
	/* inv: 	|| skips second half if first half is true, 
				so no NULL reads will occur */
	str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
	if (str == 	NULL || 0!=strcmp(str,SUBJECTCODE)) {
		X509_free(client_cert);
		return 0; 	/* need this info to get in */
	}
	OPENSSL_free(str);
	str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
	if (str == 	NULL || 0!=strcmp(str,ISSUERCODE)) {
		X509_free(client_cert);
		return 0; 	/* need this info to get in */
	}
	OPENSSL_free(str);
	/* inv:  if it got here, it must have been good! */
	X509_free(client_cert);
	return 1;
} 

int resetFailoverCount() {
	int fd = open(FAILOVERLOG, O_CREAT);
	if(fd == -1) return SYSTEMCALLFAILURE;
	write(fd, "0", 1);	
	close(fd);
	return EVERYTHINGOK;
}

int incrementFailoverCount() {
	char buffer[2];
	int fd = open(FAILOVERLOG, O_RDONLY);
	if(fd == -1) return SYSTEMCALLFAILURE;
	read(fd, buffer, 1);	buffer[1] = '\0';
	close(fd);
	(int)(buffer[0]) += 1;	/* increment before rewriting */
	fd = open(FAILOVERLOG, O_WRONLY);
	if(fd == -1) return SYSTEMCALLFAILURE;
	write(fd, buffer, 1);	
	close(fd);
	return EVERYTHINGOK;
}

int getFailoverCount() {
	char buffer[2];
	int fd = open(FAILOVERLOG, O_RDONLY);
	if(fd == -1) return -1;
	read(fd, buffer, 1);	buffer[1] = '\0'; /* just in case */
	close(fd);
	return (buffer[0]	- '0'); /* returning the ascii difference */
}

int removeString(char* buffer,int filesize,char* param) {
	int i, paramlength, start;
	char* s;
	char search[80];
	/* you want to search for the string delimited on both ends only */
	strcat(search, param);
	strcat(search, "\n"); /* all full app names end with delimiter */
	paramlength = strlen(search);
	for(start=0; start <= (filesize - paramlength); start++) {
		for(i=0; i < paramlength; i++) {
			if(buffer[start+i] != search[i]) break;
		}
		if (i == paramlength) {
			for(i=start; i < (filesize - paramlength); i++)
				buffer[i] = buffer[i + paramlength];
			filesize -= paramlength;
			return filesize; 
		}
	}
	return filesize;
}

void timestamp(char* buffer) {
	struct tm* currentTime;
	time_t now = time(NULL);
	currentTime = localtime(&now);
	strftime(buffer, 35, "%m/%d/%Y %T -", currentTime);
}

#define _XOPEN_SOURCE

int isPassword(char* password) {
	char salt[13], previous[64], together[77], c='-';
	int fd, err, i;
/* This is running as root, so it  
 * should be able to read shadow.
 */
	fd = open("/etc/shadow",O_RDONLY);
	if (fd < 0) 
		return 0;	// couldn't open file
	err = read(fd, salt, 8); /* "root:$1$" is 8 letters */
	if (err<8) 
		return 0;	// couldn't read file
	strcpy(salt, "$1$");
	i=3;
	while ( c != '$') {
		if ( (err = read(fd, &c, 1)) > 0)
			salt[i++] = c;
	}
	salt[i]='\0';
	i = 0;
	while ( c != ':') {
		if ( (err = read(fd, &c, 1)) > 0 && c != ':')
			previous[i++] = c;
	}
	previous[i]='\0';
	close(fd);
	strcpy(together, salt);
	strcat(together, previous);
	return ( 0 == strcmp(together, crypt(password,salt) ) ); 
}

int setPassword(char* password) {
	char salt[13];
	FILE* fp;
	int i;
/* initialize the random function */
	srandom(time(NULL));
/* This is running as root, so it  
 * should be able to overwrite
 * shadow.
 * crypt requires an up to 8 character salt that 
 * has characters in the set [a-zA-Z0-9./]       
 * since we're using MD5 passwords we prefix our 
 * salt with $1$ and end it with $ 					 
 */
	salt[0] = '$';
	salt[1] = '1';
	salt[2] = '$';
	for (i = 3; i < 11; i++) {
   	do {
      salt[i] = (random() % 64) + '.';
   	} while (((salt[i] > '9') && (salt[i] < 'A')) ||
           ((salt[i] > 'Z') && (salt[i] < 'a')));
	}
	salt[11] = '$';
	salt[12] = '\0';

/* Shadow format <user>:<encrypted pass>:
 * <time since epoch when changed (in days)>:<unused fields> */
	fp = fopen("/etc/shadow", "w");
	if (fp == NULL)
		return -1;
	fprintf(fp, "root:%s:%ld::::::\n", crypt(password, salt),
								        (unsigned long)(time(NULL)/(60*60*24)));
	fclose(fp);
	return 0;
}



	
#endif
