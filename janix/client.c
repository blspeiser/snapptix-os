#include "Janix.h"
#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

int main (int argc, char *argv[]) {
  int err;
  int sd;
  struct sockaddr_in sa;
  X509*    server_cert;
  char*    str;
  char 	  msg[PROTOCOLMAXSIZE];

  out = stdout;

  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();
  err = loadSSL(SSLv3_client_method(), CERTF, KEYF, PASSPHRASE);
  if (err != EVERYTHINGOK) criticalFailure(err, "Could not load SSL\n");

  sd = socket (AF_INET, SOCK_STREAM, 0); CHK_ERR(sd, "socket"); 
  memset (&sa, '\0', sizeof(sa));
  sa.sin_family      = AF_INET;
  sa.sin_addr.s_addr = inet_addr ("127.0.0.1");   /* Server IP */
  sa.sin_port        = htons     (1111);          /* Server Port number */
  err = connect(sd, (struct sockaddr*) &sa,sizeof(sa));CHK_ERR(err, "connect");
  ssl = SSL_new(ctx);CHK_NULL(ssl);    
  SSL_set_fd(ssl, sd);
  err = SSL_connect(ssl);CHK_SSL(err); 
  
/*  validateClient();  /* This does display the Server certificate */
  /* --------------------------------------------------- */
  /* DATA EXCHANGE - Send a message and receive a reply. */
  /* --------------------------------------------------- */

  if (argc == 1)
		strcpy(msg,"E-Echo Test");
  else 
		strcpy(msg, argv[1]);

  err = SSL_write (ssl, msg, strlen(msg));	  						CHK_SSL(err);
  err = SSL_read  (ssl, msg, PROTOCOLMAXSIZE - 2);           	CHK_SSL(err);
  msg[err] = '\0';
  printf("Reply: %s (%d)\n", msg, err);

  /* --------------------------------------------------- */

  /* Clean up. */
  SSL_shutdown(ssl);  /* send SSL/TLS close_notify */
  closeRequest(EVERYTHINGOK); /* calls exit(0) */
  return NOEXIT; /* Really bad if it gets here */
}
/* EOF - client.c */
