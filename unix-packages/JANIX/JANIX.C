#include "Janix.h"
/**************************************************************
	B. L. Speiser - Janix.c - Janix Server for Snappware
	Release Candidate 3 (Beta release 4.1)
	Last made changes: 4/2/2003
**************************************************************/
int main(int argc, char* argv[]) {
	int err;									
	int listen_sd;							
	struct sockaddr_in sa_serv;		
	struct sockaddr_in sa_cli;			
	size_t client_len;					
	char *str;								
	char buf[MAXSOCKETSIZE];
	int fd;
	int pid;
	int procpid;
	int trustworthy;
	char path[PROTOCOLMAXSIZE];
	char response[PROTOCOLMAXSIZE];
	char pidstring[10];
	struct stat filedata;
	struct dirent* directory;
	DIR* dirp;

	/* Establish output location */
	if(argc>1) {
		out = fopen(argv[1],"a+");			/* a+ := Append (or Create) */
		if (out == NULL) out = stdout;
	}
	else {
/***RYAN - uncomment next two lines when ready for production***/
/*		out = fopen(SERVERLOG,"a+");	
		if (out == NULL) */ 
			out = stdout;	/* right now, just output to screen if not specified */
	}

	/* First things first - write a pid file*/
	timestamp(response);
	fprintf(out, response);
	if ( (fd=creat(MYPIDFILE, O_TRUNC)) == -1 ) {
		fprintf(out, " creat() failed:\tProgram requires root access.\n");
		fclose(out);
		exit(EVERYTHINGOK);
	}
	chmod(MYPIDFILE, 0644); /* As per Ryan's recommedation */
	sprintf(pidstring,"%d",getpid());
	fprintf(out, " pid: %s\n", pidstring);
	write(fd,pidstring,strlen(pidstring));
	close(fd);	
	
	/* SSL preliminaries. We keep the certificate and key with the context. */ 
    
	SSL_load_error_strings();					
	SSLeay_add_ssl_algorithms();				
	switch ( loadSSL(SSLv23_server_method(),CERTF,KEYF,PASSPHRASE) ) {
		/* No need for breaks: things should never continue after critical failures*/
		case SSLERROR: 	
			criticalFailure(SSLERROR, 	  	"Cannot start SSL encryption\n");
		case MISSINGCERT:	
			criticalFailure(MISSINGCERT, 	"Missing SSL Certificate\n");
		case MISSINGPK: 
			criticalFailure(MISSINGPK, 	"Missing SSL Key file\n");
		case WRONGCERT:
			criticalFailure(WRONGCERT,		"Private and public keys do not match\n");
		case EVERYTHINGOK:
			break;	/* Everything ok */
		default:
			DEBUGLOGICERROR; /* should never get here!!! */ 
	};
	/* Force client certificate requests */
//	SSL_CTX_load_verify_locations(ctx, CERTF, NULL);
//	SSL_CTX_set_verify(	ctx, 
//								SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
//								NULL);
//	SSL_CTX_set_verify_depth(ctx, 1); /* 1 := Require to be signed 
//													by a Trusted CA, ie Us */

	/* Prepare failover count */
	if( resetFailoverCount() ) criticalFailure(SYSTEMCALLFAILURE, "Could not open failover log\n");

	/* Prepare TCP socket for receiving connections */ 
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (err == -1) criticalFailure(SYSTEMCALLFAILURE, "socket() failed\n");

	memset(&sa_serv, '\0', sizeof(sa_serv));
	sa_serv.sin_family = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port = htons(UNIXPORT);	/* Server Port number */

	err = bind(listen_sd, (struct sockaddr *) &sa_serv, sizeof(sa_serv));
	if (err == -1) criticalFailure(SYSTEMCALLFAILURE, "bind() failed\n");

	/* Start SSL Server */ 
	while (SSL_SERVER_LOOP) { 
		err = listen(listen_sd, MSGQUEUESIZE);
		if (err == -1) criticalFailure(SYSTEMCALLFAILURE, "listen() failed\n"); 
		client_len = sizeof(sa_cli); 
		/* Server pauses here to wait for actual incoming message */
		sd = accept(listen_sd, (struct sockaddr *) &sa_cli, &client_len);
		if (sd == -1) criticalFailure(SYSTEMCALLFAILURE, "accept() failed\n");
		/* Now that we finally got a message, fork and deal with it */
		pid = fork();
		if (pid < 0) criticalFailure(SYSTEMCALLFAILURE, "fork() failed\n");
		if (0 == pid) {	/* Child */
			/* Handle request */ 
			close(listen_sd);
			/* ----------------------------------------------- */ 
			/* TCP connection is ready. Do server side SSL. 	*/ 
	
			ssl = SSL_new(ctx);
			if (ssl == NULL) criticalFailure(SSLERROR, "SSL_new failed\n");

		  	SSL_set_fd(ssl, sd);  			err = SSL_accept(ssl);
			if (err == -1) {
				timestamp(response);
				fprintf(out, "%s SSL_accept failed: Unauthorized client or SSL error\n", response);
				closeRequest(EVERYTHINGOK);
			}
	
			/* otherwise; inv: client has been properly authenticated */

			/* DATA EXCHANGE - Receive mesfailoverRequestssage and send reply. */ 
			err = SSL_read(ssl, buf, sizeof (buf) - 1);
			if (err == -1) criticalFailure(SSLERROR, "SSL_read failed\n");
		
		   /* SECURITY: Prevent buffer overruns or protocol corruptions */ 
			if (err > (PROTOCOLMAXSIZE-1) || err < 0) {
					err = 0;	/* Insure that this will cause no damage */
			}
			/* inv:  0 <= err <= PROTOCOLMAXSIZE-1 (63) */ 
			buf[err] = '\0'; /* All strings read from sockets must be terminated */

			/*--OUTPUT--*/
			timestamp(response);
			fprintf(out,"%s Received: %s (%d)\n", response, buf, err); 
		      
			/***** Handler code *****/
			switch(HEADER) {
			case PROCESS:		  
				if( resetFailoverCount() ) criticalFailure(SYSTEMCALLFAILURE, "Could not open failover log\n");
				/*--DEFINE PROTOCOL--*
				 *    Incoming:   "P*xxxx....\0"
				 *    xxxx	:= Process Name; Bayo is supposed to have this info
				 *    *		:= K | R | - 
				 *		Implementation Status: Complete. Ready for release.
				 *--END PROTOCOL--*/
			
				if (COMMAND != KILL && COMMAND != RESTART && COMMAND != SKIP) {
						strcpy(response, PROTOCOLERROR);
						break;	/* All done */
				}
	
				/* Find process pid. Remember:  Ryan guarenteed no
					path would be longer than 64 chars; so this works
					without error checking 
				*/
				strcpy(path, PIDPATH);
				strcat(path, PARAMETER);
				strcat(path, PIDEXT);
			  	fd = open(path, O_RDONLY);
				if (fd == -1) {
					if (COMMAND == SKIP) {
						strcpy(response, NO);	/* could not find it */
						break;	/* All done */
					}
					if (COMMAND == KILL || COMMAND == RESTART) {
						/*--SECURITY--*/ 
					   strcpy(response, FAILED);	/* not good if you get here */
						break;	/* All done */					}	
					DEBUGLOGICERROR; /* should never get here!!! */ 
				} 
				/* inv: if here, pid file was found */
			  	err = read(fd, pidstring, sizeof(pidstring));
				if (err == -1) {
					strcpy(response, FAILED);	/* Wacky read error */
					break;	/* All done */
				}
				/* inv:  pid file found and read; now check if still active:
							look for it in the proc file system.  If there is 
							directory of the corresponding pid, then it is still
							active. */
				strcpy(path, PROCFSO);
				/* pid may not be trimmed of whitespace*/
				for(str = pidstring; str - pidstring < 10; str++)
					if (*str < '0' || *str > '9') *str='\0';
				for(str = pidstring; *str < '0' || *str > '9'; str++);	
				/* inv: str points to pid section only */
				strcat(path, str);	
				stat(path, &filedata);
				if ( (S_IFMT & filedata.st_mode) != S_IFDIR) {
					/* pid file was left over, but process has been killed for a while */
					if (COMMAND == SKIP) {
						strcpy (response, NO);	/* could not find it */
						break;	/* All done */
					}
					if (COMMAND == KILL || COMMAND == RESTART) {
					   strcpy(response, FAILED);	/* not good if you get here */
						break;	/* All done */
 					}	
					DEBUGLOGICERROR; /* should never get here!!! */ 
				} /* End leftover pid file */
				/* inv: there is such a directory; i.e. process is active */
				if (COMMAND == SKIP) {
					strcpy(response, YES);
			   	break;	/* All done */
				}
				/* otherwise... */
				if (0==strcmp(PROGRAM,PARAMETER)){ /* check for suicide */
					   strcpy(response, FAILED);	/* Do not allow */
						timestamp(path);
						fprintf(out, "%s Suicide attempt averted.\n", path);
						break;	/* All done */
				}	
				/* otherwise... */
				procpid = atoi(str);
				if (procpid<1) criticalFailure(SYSTEMCALLFAILURE, "process has pid < 1\n");
   			/* inv: pid is valid and active */
				if (COMMAND == KILL) {
					err = kill(procpid, SIGINT);
      			if (err == -1) { 
						timestamp(path);
						fprintf(out, "%s kill -INT failed when terminating %d\n", path, procpid);
						err = kill(procpid, SIGQUIT);
						if (err == -1) { 
							timestamp(path);
							fprintf(out, "%s kill -QUIT failed when terminating %d\n", path, procpid);
							err = kill(procpid, SIGKILL);
							if (err == -1) { 
								strcpy(response, FAILED);
								timestamp(path);
								fprintf(out, "%s kill -9 failed when terminating %d\n", path, procpid);
								break;	/* All done */
							}
						}
					}
					/* otherwise... */
					strcpy(response, SUCCEEDED);
		     		break;	/* All done */
				} 
				if (COMMAND == RESTART) { /* by logic, only possibility left, but check anyway */
					/* Send hangup signal to tell them to restart */ 
					err = kill(procpid, SIGHUP);
			  		if (err == -1) {
						strcpy(response, FAILED);
			  			break;	/* All done */
					}
					/* otherwise... */
					strcpy(response, SUCCEEDED);
					break;	/* All done */
				}
				DEBUGLOGICERROR; /* should never get here!!! */
				break;
			/* End PROCESS | RESTART | KILL Handler */ 

			case APP:
				if( resetFailoverCount() ) criticalFailure(SYSTEMCALLFAILURE, "Could not open failover log\n");
		  		/*--DEFINE PROTOCOL--*
				 *      Incoming:   "X*xxxx....\0"
				 *      xxxx   = Package name
				 *      * = I | U | -
				 *          I: Install application module
				 *          U: Uninstall application module
				 *          -: Is module installed or not? 
				 * 			Implementation Status: AppQuery & AppInstall complete,
				 *												but still dealing with 
				 *												modifying APPLOG during 
				 *												uninstall.
				 *--END PROTOCOL--*/ 

				if (COMMAND != INSTALL && COMMAND != UNINSTALL && COMMAND != SKIP) {
			  		strcpy (response, PROTOCOLERROR);
			  		break;	/* All done */
			  	}
				timestamp(path);
				fd = open(APPLIST, O_RDWR | O_APPEND | O_CREAT);
				if (fd == -1) {
					fprintf(out, "%s %s not found\n", path, APPLIST);
					strcpy(response, FAILED);
			  		break;	/* All done */
			  	}
				/* parse APPLOG, find the app name.  If there, installed;
					if not there, not installed. */
				strcpy(response, NO);
				while( (err=readln(fd, path, PROTOCOLMAXSIZE-1)) == READ_OK) {
					if(err == READ_ERR) { 
						strcpy(response, FAILED);
						break; 
					}
					/* otherwise... */
					if(0==strcmp(path,PARAMETER)){/* found it installed in applog */
						strcpy(response, YES);
						break;
					}
				}
				/* inv: if not found, then response = NO or FAILED */
	    	
				if (  (response[0] == 'Y' && COMMAND==INSTALL) 	||
						(response[0] == 'N' && COMMAND==UNINSTALL)||
						(response[0] == 'F' 								))
					{ 
					close(fd);
					strcpy(response, FAILED);
			  		break;	/* All done */
			  	} 
				/* otherwise... */
				if (COMMAND == SKIP) {
					close(fd);
					break;	/* Answer already prepared; all done */
				}
				if (COMMAND == INSTALL) { /* Logical, but check to make sure */
					/* first, check to make sure package is available for install */
					dirp = opendir(PACKAGELOCATION);
					if (dirp == NULL) { 
						close(fd);
						strcpy(response, FAILED);
				  		break;	/* All done */
				  	} 
					while( directory = readdir(dirp) ) {
						/* Test for bad directory name */
						if (directory->d_ino == 0 || !directory->d_name[0]) continue;
						/* otherwise... */
						/* compare parameter to package name, ignoring version number */
						for(err=0; err < strlen(PARAMETER); err++) {
							if( directory->d_name[err] != *(PARAMETER+err) ) break;
						}
						if(err < strlen(PARAMETER)) continue; /* did not find match */
						/* inv: if it gets here, it just found it! */
						break;
					}
					closedir(dirp);
					if( directory == NULL ) { /* did not find match */
						close(fd);
						strcpy(response, FAILED);
						break;
					}
					/* inv: package found and will be installed. */
					strcpy(path, PACKAGELOCATION);
					strcat(path,directory->d_name);	/* Add full file name */		
					free(directory);
					pid = fork();
					if (pid < 0) criticalFailure(SYSTEMCALLFAILURE, "fork() failed\n");
					if (0 == pid) { /* Grandchild */ 
						execlp(PKGINSTALLPATH, PKGINSTALLPATH, path, NULL);
						criticalFailure(SYSTEMCALLFAILURE, "exec returned to code that called it\n");
					}
					waitpid(pid, &err, 0);
					if (err == EVERYTHINGOK) { 
						/* add to applist */
						write(fd, PARAMETER, strlen(PARAMETER));
						write(fd, "\n", 1);
						close(fd);
						strcpy(response, SUCCEEDED);
						break;	/* All done */
					} 
					/* inv: if it got here, then install failed */
					close(fd);
					strcpy(response, FAILED);
					break;	/* All done */
		    	} /* End install */
				if (COMMAND == UNINSTALL) {
					pid = fork();
					if (pid < 0) criticalFailure(SYSTEMCALLFAILURE, "fork() failed\n"); 
					if (0 == pid) {	/* Grandchild */ 
						execlp(PKGUNINSTALLPATH, PKGUNINSTALLPATH, PARAMETER, NULL);
						criticalFailure(SYSTEMCALLFAILURE, "exec returned to code that called it\n");
					}
					waitpid(pid, &err, 0);
					if (err == EVERYTHINGOK) {
						/* remove from applist */
						close(fd);	/* close & reopen to go back to beginning of file */
						fd = open(APPLIST, O_RDONLY);	
						if (fd == -1) {
							strcpy(response, FAILED);
			  				break;	/* All done */
			  			}
						/* C can't declare vars midstream; so I get a 
							little techy with the vars that I use.  Be 
							really careful if editing the code; I didn't
							want to add extra vars; so I use err := file
							size and fd = a[fd]
						*/		
						strcpy(path, PARAMETER); /* store parameter */
						/* No way applog will be >= 4 KB; store in buf */
						err = read(fd, buf, MAXSOCKETSIZE-1);
						if (err == -1) {
							strcpy(response, FAILED);
			  				break;	/* All done */
			  			}
						close(fd);
						/* inv: program can be listed only once (fail on multiple install) */
						/* err:= size of file before removal */
						err = removeString(buf, err, path);
						/* err:= size of file after removal */
						fd = open(APPLIST, O_WRONLY | O_TRUNC );
						if (fd == -1) {
							strcpy(response, FAILED);
			  				break;	/* All done */
			  			}
						write(fd, buf, err);
						close(fd);
						/* inv: everything completed */
						strcpy(response, SUCCEEDED);
						break;	/* All done */
					} 
					/* inv: if it got here, then uninstall failed */
					strcpy(response, FAILED);
					break;	/* All done */
		    	} /* End uninstall */
				DEBUGLOGICERROR; /* should never get here! */
				break;
	  		/* End APP | INSTALL | UNINSTALL Handler */ 
			
			case MANAGE:
				/*--DEFINE PROTOCOL--*
				 *     Incoming:   "M*----\0"
				 *     *  = N | B
				 *		 -- = Command line options for script
				 *          B: Configure backup options
				 *          N: Configure network settings
				 *
				 * 			Implementation Status: Just started, untested
				 *--END PROTOCOL--*/ 
				if(COMMAND != NETWORK && COMMAND != BACKUP) {
					strcpy(response, PROTOCOLERROR);
					break;
				}
				/* otherwise */
				if	(COMMAND==NETWORK)
					strcpy(path, IPSCRIPT);
				else if(COMMAND==BACKUP)	/* Logical, but check to make sure */
					strcpy(path, EXPORTSCRIPT);
				else 
					{ DEBUGLOGICERROR; }
				pid = fork();
				if (pid < 0) criticalFailure(SYSTEMCALLFAILURE, "fork() failed\n");
				if (0 == pid) { /* Grandchild */ 
					execlp(path, path, PARAMETER, NULL);
					criticalFailure(SYSTEMCALLFAILURE, "exec returned to code that called it\n");
				}
				waitpid(pid, &err, 0);
				if(err)
					strcpy(response, FAILED);
				else
					strcpy(response, SUCCEEDED);
				break;
			/* End MANAGE | NETWORK | BACKUP Handler */

			case ALPHA:
				/*--DEFINE PROTOCOL--*
				 *      Incoming:   "A*\0"
				 *      * = ? | K
				 *          ?: Return Alpha rank from Linux-HA 
				 *          K: Cause a deliberate failover
				 * 			Implementation Status: AlphaKill complete;
				 * 											AlphaQuery waiting for Ryan
				 *--END PROTOCOL--*/ 

				if (COMMAND != STATUSREQ && COMMAND != KILL) {
		    		strcpy(response, PROTOCOLERROR);
	      		break;	/* All done */
				}
		  		if (COMMAND == STATUSREQ) {	
					/* Restart failover count; received non-failover command */
					if( resetFailoverCount() ) criticalFailure(SYSTEMCALLFAILURE, "Could not open failover log\n");
					/* Ryan */
					/* retrieve info from Linux-HA */ 
					/* if can't get info, */ {
						strcpy(response, FAILED); 
						break;	/* All done */
					}
		  			/* otherwise... */ 
					/* if Alpha unit, */	{
						strcpy(response, ALPHAUNIT);
						break;	/* All done */
		      	}	
					/* inv: must be beta unit */ 
					strcpy(response, BETAUNIT);
					break;	/* All done */
				}
				if (COMMAND == KILL) {		/* Logical, but check to make sure */
					if( incrementFailoverCount() ) criticalFailure(SYSTEMCALLFAILURE, "Could not open failover log\n");
					err = getFailoverCount();
					timestamp(path);
					fprintf(out, "%s Failover message count at %d\n", path, err);
					if (err > FAILOVERTHRESHOLD) {
						closeRequest(NOEXIT);
						criticalFailure(DELIBERATEREBOOT, "Deliberate failover triggered\n");
						/* Will never continue from here */
					}
					/* otherwise don't respond; wait for more deliberate failovers.
						 Sending a response might validate a brute force 
						 entry in an attempted security breach. 
					*/ 
			    	strcpy(response, EMPTYRESPONSE);
			  		break;	/* All done */
				}
				DEBUGLOGICERROR; /* should never get here! */
				break;
			/* End ALPHA | STATUSREQ | KILL Handler */ 
				
			case ECHO:
				if( resetFailoverCount() ) criticalFailure(SYSTEMCALLFAILURE, "Could not open failover log\n");
			 	/*--DEFINE PROTOCOL--*
				 *		Incoming:   "E-xxxx....\0"
				 *		xxxx   = Echo text
				 *		Implementation Status: Complete.  Ready for release.
				 *--END PROTOCOL--*/ 
			 
				if (COMMAND != SKIP) {
			     	strcpy(response, PROTOCOLERROR);
			     	break;	/* All done */
			   }
			   /* inv: Incoming parameter MUST be less than 64 chars to not be automatically 
			     	 rejected before the handler process.  Because the protocol defines the 
			     	 incoming and outgoing data to be identical and both under 64 chracters, */ 
				strcpy(response, buf);	/* return the exact incoming message */
		  		break;	
			/* End ECHO Handler */
	
			case NOCOMMAND:
				/* If you don't return a write, it can the sample client to hang */
			  	strcpy(response, EMPTYRESPONSE); 
				break;
			/* End NOCOMMAND Handler */ 
	
			default:
				/* If we got here, then an invalid command was sent */ 
				strcpy(response, PROTOCOLERROR);
				break;	/* Make compilers happy; technically unnecesary */
			} /* End switch(HEADER) */
		      
			/* inv: incoming data was handled; reply was constructed in the string response */ 
	  		/* Reply with constructed answer */ 
			if(response[0] != SKIP) {	/* ie not an empty response */
				err = SSL_write(ssl, response, strlen(response));
				if (err == -1) criticalFailure(SSLERROR, "SSL_write failed\n");
			}
		   /* Child socket replied, so all done. Clean up and go home. */ 
		   closeRequest(EVERYTHINGOK); /* calls exit(); */
					
		} /* End Child - go back to Parent: */ 
	
		/* Signal handling in SSLserv server code */ 
		signal (SIGCLD, SIG_IGN);		/* Death of children should do nothing */
		signal (SIGINT, sigcatcher);	/* Currently just resets the signal handlers */
	} /* End while(SSL_SERVER_LOOP) */
		
	/* Application should not get here! */ 
   closeRequest(LOGICERROR);  /* calls exit(); */
	return NOEXIT; /* REALLY bad if you get here */
}
/* EOF - Janix.c */ 
