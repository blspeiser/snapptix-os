--- Janix.c.orig	2003-04-22 00:05:06.000000000 -0500
+++ Janix.c	2003-04-21 17:36:59.000000000 -0500
@@ -69,12 +69,12 @@
 			DEBUGLOGICERROR; /* should never get here!!! */ 
 	};
 	/* Force client certificate requests */
-	SSL_CTX_load_verify_locations(ctx, CERTF, NULL);
-	SSL_CTX_set_verify(	ctx, 
-								SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
-								NULL);
-	SSL_CTX_set_verify_depth(ctx, 1); /* 1 := Require to be signed 
-													by a Trusted CA, ie Us */
+//	SSL_CTX_load_verify_locations(ctx, CERTF, NULL);
+//	SSL_CTX_set_verify(	ctx, 
+//								SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
+//								NULL);
+//	SSL_CTX_set_verify_depth(ctx, 1); /* 1 := Require to be signed 
+//													by a Trusted CA, ie Us */
 
 	/* Prepare failover count */
 	if( resetFailoverCount() ) criticalFailure(SYSTEMCALLFAILURE, "Could not open failover log\n");
@@ -273,7 +273,7 @@
 			  		break;	/* All done */
 			  	}
 				timestamp(path);
-				fd = open(APPLIST, O_RDWR | O_APPEND);
+				fd = open(APPLIST, O_RDWR | O_APPEND | O_CREAT);
 				if (fd == -1) {
 					fprintf(out, "%s %s not found\n", path, APPLIST);
 					strcpy(response, FAILED);
@@ -344,7 +344,7 @@
 						execlp(PKGINSTALLPATH, PKGINSTALLPATH, path, NULL);
 						criticalFailure(SYSTEMCALLFAILURE, "exec returned to code that called it\n");
 					}
-					waitpid(pid, &err);
+					waitpid(pid, &err, 0);
 					if (err == EVERYTHINGOK) { 
 						/* add to applist */
 						write(fd, PARAMETER, strlen(PARAMETER));
@@ -365,7 +365,7 @@
 						execlp(PKGUNINSTALLPATH, PKGUNINSTALLPATH, PARAMETER, NULL);
 						criticalFailure(SYSTEMCALLFAILURE, "exec returned to code that called it\n");
 					}
-					waitpid(pid, &err);
+					waitpid(pid, &err, 0);
 					if (err == EVERYTHINGOK) {
 						/* remove from applist */
 						close(fd);	/* close & reopen to go back to beginning of file */
@@ -438,7 +438,7 @@
 					execlp(path, path, PARAMETER, NULL);
 					criticalFailure(SYSTEMCALLFAILURE, "exec returned to code that called it\n");
 				}
-				waitpid(pid, &err);
+				waitpid(pid, &err, 0);
 				if(err)
 					strcpy(response, FAILED);
 				else
--- utils.h.orig	2003-04-22 00:05:16.000000000 -0500
+++ utils.h	2003-04-21 17:51:18.000000000 -0500
@@ -173,20 +173,19 @@
 	int i, paramlength, start;
 	char* s;
 	char search[80];
-	/* you want to search for the string delimited on both ends only */
-	strcpy(search, "\n");	
-	strcat(search, param);
+
+	strcpy(search, param);
 	strcat(search, "\n"); /* all full app names end with delimiter */
 	paramlength = strlen(search);
-	for(start=0; start < filesize - paramlength; start++) {
+	for(start=0; start <= (filesize - paramlength); start++) {
 		for(i=0; i < paramlength; i++) {
 			if(buffer[start+i] != search[i]) break;
 		}
 		if (i == paramlength) {
-			start++; /* skip the extra \n */
-			for(i=0; i<paramlength-1; i++)
-				buffer[start+i] = '\b';	/* delete text! */
-			filesize -= (paramlength-1); /* did not delete first \n*/
+			for(i=start; i< (filesize - paramlength); i++)
+				buffer[i] = buffer[i + paramlength];
+			filesize -= paramlength;
+			return filesize;
 		}
 	}
 	return filesize;
