package Snapptix_backend;
import java.io.*;
import java.net.*;
import javax.net.*;
import javax.net.ssl.*;
import java.security.*;
import java.security.cert.*;
import java.util.*;
/*** 


		THE ONLY WAY TO DO THIS IS THROUGH AN UNSECURED WORKAROUND 
                            THIS CODE IS A MAJOR JAVA SSL HACK					
				DO NOT EDIT ANYTHING
7
                            - B. L. Speiser, 4/6/2003

***/
	
/***********

How to use:

SSLcomm SSL = new SSLcomm(HOST,PORT); 
		// if not specified, will default to localhost:1111

String message;
SSL.write(message);			// <- Establishes connection before writing
String reply = SSL.read();	// <- Server closes connection after read!


***********/


public class SSLcomm implements X509TrustManager {
	final public static String  DEFAULT_HOST = "127.0.0.1";
	final public static int     DEFAULT_PORT = 1111;

	private SSLSocket   sd;
	private String      JanixHost;
	private int         JanixPort;

	public SSLcomm(String host, int port) {
		this.JanixHost = host;
		this.JanixPort = port;

		// Prepare environment variables
		System.setProperty("java.protocol.handler.pkgs",
				"com.sun.net.ssl.internal.www.protocol"
		);
		java.security.Security.addProvider(
			new com.sun.net.ssl.internal.ssl.Provider()
		);
		this.sd = null;
	}

	public SSLcomm() {
		this.JanixHost  = SSLcomm.DEFAULT_HOST;
		this.JanixPort  = SSLcomm.DEFAULT_PORT;

		// Prepare environment variables
		System.setProperty("java.protocol.handler.pkgs",
				"com.sun.net.ssl.internal.www.protocol"
		);
		java.security.Security.addProvider(
			new com.sun.net.ssl.internal.ssl.Provider()
		);
		this.sd = null;
	}
	
	public void write(String message) {
		try {
			this.sd = SSLcomm.getSSLconnection(this.JanixHost, this.JanixPort);
			if(this.sd == null) 
				throw new IOException("SSL socket not constructed");
			Writer out = new OutputStreamWriter(sd.getOutputStream(), "ASCII");
			out.write(message);
			out.flush();
                } catch(IOException e) {
			System.out.println(e);
		}
	}

	public String read() {
		try{
			char[] buffer = new char[64];
			Reader in = new InputStreamReader(sd.getInputStream(), "ASCII");
			in.read(buffer);
			String response = String.valueOf(buffer);
			return response;
		} catch(Exception e) {
			return new String("F-");	// ie Failed
		}
	}
	
	protected void finalize() {
		try {
			sd.close();
		} catch(IOException e) {
			System.out.println(e);
		}
	}

	protected static SSLSocket getSSLconnection(String host, int port) {
		try {
			// Start the Hack using our workaround class 
			X509TrustManager tm  = new SSLcomm(host, port);
			KeyManager[] 	 km  = null;
			TrustManager[] 	 tma = {tm};
			SSLContext 	 ctx = SSLContext.getInstance("SSL");
			ctx.init(km,tma,new java.security.SecureRandom());
			SSLSocketFactory    factory = ctx.getSocketFactory();
			Socket              sd  = new Socket(host,port);
			SSLSocket           sdX = (SSLSocket)factory.createSocket(sd,host,port,true);
			sdX.startHandshake();
			return sdX;
		} catch (Exception e) {	
			System.out.println(e);
			return null;
                }
	}

	public boolean isClientTrusted(java.security.cert.X509Certificate[] chain){ 
		return true;
	}
	
        public boolean isServerTrusted(java.security.cert.X509Certificate[] chain){ 
		for(int i=0; i < chain.length; i++) {
			X509Certificate cert = chain[i];
			System.out.println("Subject: " + cert.getSubjectDN().getName() );
			System.out.println("Issuer:  " + cert.getIssuerDN().getName()  );
		}
		return true;
	}
        
        public void checkClientTrusted(java.security.cert.X509Certificate[] chain,
            String check){ }
        
        public void checkServerTrusted(java.security.cert.X509Certificate[] chain,
            String check){ }
        
	public X509Certificate[] getAcceptedIssuers() { 
		return null; 
	}
}

