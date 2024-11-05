#ifndef __SNAPPTIX__constants_h
#define __SNAPPTIX__constants_h

#define RELEASE
/* enables reboot options and logging to standard log */

/* Protocols Stuff:  Incoming are chars, Outgoing are strings / Mehul Bayo */ 
#define PROCESS				'P'
#define RESTART				'R'         
#define KILL					'K'
#define APP						'X' /* as in "eXecutable"   */
#define INSTALL				'I'         
#define UNINSTALL				'U' 
#define ALPHA					'A'
#define STATUSREQ				'?'
#define ECHO					'E'
#define MANAGE					'M'
#define NETWORK				'N' 
#define EMAIL					'E'
#define PASSWORD				'P'
#define BACKUP					'B'
#define MOUNT					'M'
#define UNMOUNT				'U'
#define SKIP					'-'
#define NOCOMMAND				'\0'
#define PROTOCOLERROR		"X-"
#define YES						"Y-"
#define NO						"N-"
#define SUCCEEDED				"S-"
#define FAILED					"F-"
#define ALPHAUNIT				"A-"
#define BETAUNIT				"B-"
#define ECHOHEADER			"E-"
#define EMPTYRESPONSE		"--"
#define PROTOCOLMAXSIZE		64
#define MAXSOCKETSIZE		4096
#define HEADER					buf[0]
#define COMMAND				buf[1]
#define PARAMETER				&(buf[2]) /* ie convert rest to standard char* string */
#define FAILOVERTHRESHOLD	3 /* # commands needed to cause a deliberate failover */
#define UNIXPORT				1111
#define IJAVAPORT				6789	/* currently not needed */
#define OJAVAPORT				9876	/* currently not needed */
#define MSGQUEUESIZE			5
#define CIPHER					"DES-CBC3-MD5"
#define PASSPHRASE			"Snappware"

/* Pathnames */ 
#define PROGRAM				"Janix"
#define PIDEXT					".pid"
#define LOGEXT					".log"
#define HOME 					"/usr/share/Janix/"
#define CERTHOME				"/etc/ssl/certs/"
#define PROCFSO				"/proc/"
#define PIDPATH				"/var/run/"
#define PKGINSTALLPATH		"/usr/bin/pkgadd"
#define PKGUNINSTALLPATH	"/usr/bin/pkgrm"
#define PACKAGELOCATION		"/crux/opt/"
#define SERVERLOG				"/var/log/" PROGRAM LOGEXT
#define MYPIDFILE				PIDPATH PROGRAM PIDEXT
#define FAILOVERLOG			HOME "failover" LOGEXT
#define APPLIST				HOME "apps" LOGEXT
#define CERTF					CERTHOME "SnappCert.pem"	/* cert file */
#define KEYF  					CERTHOME "SnappCert.pem"	/* key  file */
#define IPSCRIPT				HOME "setip.sh"
#define EXPORTSCRIPT			HOME "backup.sh"
#define EMAILSCRIPT			HOME "email.sh"

/* exit() status codes */ 
#define EVERYTHINGOK			0
#define SYSTEMCALLFAILURE	1
#define SSLERROR				2
#define MISSINGCERT			3
#define MISSINGPK				4
#define WRONGCERT				5
#define LOGICERROR			6
#define DELIBERATEREBOOT	7
#define NOEXIT					-1

/* Readability stuff */ 
#define SSL_SERVER_LOOP		1		/* boolean true for readability and searches */
#define READ_ERR 		2
#define READ_DONE 	1	/* readln() stuff */
#define READ_OK 		0

/* Credentials */
#define SUBJECTCODE "/C=US/ST=New Jersey/L=Newark/O=Snapptix Consulting/OU=Janix Development/CN=B. L. Speiser/Email=blspeiser@yahoo.com"
#define ISSUERCODE  "/C=US/ST=New Jersey/L=Newark/O=Snapptix Consulting/OU=Janix Development/CN=B. L. Speiser/Email=blspeiser@yahoo.com"

/* Miscellaneous */
#define _XOPEN_SOURCE /* need to define in order to use crypt() */

#endif
