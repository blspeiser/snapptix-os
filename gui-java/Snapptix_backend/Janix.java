package Snapptix_backend;

import java.io.*;
import java.net.*;
import java.util.*;


public class Janix implements GlobalIP {
    protected SSLcomm   SSL         = null;
    protected String    response    = null;

    //Constructors
    public Janix() {
      try {
        SSL = new SSLcomm(localHost, JanixPort);
      } catch(Exception e) {
        System.out.println(e);
        System.exit(1);
      }
    }

    public Janix(String host, int port) {
      try {
        SSL = new SSLcomm(host, port);
      } catch(Exception e) {
        System.out.println(e);
        System.exit(1);
      }
    }
    public Janix(int port) {
      try {
        if (port != 0) // allows for running without Janix server actually running
          SSL = new SSLcomm(localHost, port);
      } catch(Exception e) {
        System.out.println(e);
        System.exit(1);
      }
    }
    //Accessors
    public boolean isAlpha() {
        response = sendMessage(ALPHA + STATUSREQ);
        return strcmp(response, ALPHAUNIT);
    }
    public boolean isBeta(){
        response = sendMessage(ALPHA + STATUSREQ);
        return strcmp(response, BETAUNIT);
    }
    public boolean isApplicationInstalled(String app){
        response = sendMessage(APP + SKIP + app);
        return strcmp(response, YES);
    }
    public boolean isProcessActive(String proc){
        response = sendMessage(PROCESS + SKIP + proc);
        return strcmp(response, YES);
    }
    public boolean testConnection() {
        String s = "Testing Janix communication link...";
        if ( echo(s) )
            return false;
        //otherwise
        return true;
    }
    public String  getResponse() {
        return response;
    }

    //Mutators: return true on error!
    public boolean changePassword(String oldpw, String newpw) {
      // send old and new separated by a # sign
      response = sendMessage(MANAGE + PASSWORD + oldpw + "#" + newpw);
      return !(strcmp(response, SUCCEEDED));
    }
    public boolean mount(String fso) {
      response = sendMessage(MANAGE + MOUNT + fso);
      return !(strcmp(response, SUCCEEDED));
    }
    public boolean unmount(String fso) {
      response = sendMessage(MANAGE + UNMOUNT + fso);
      return !(strcmp(response, SUCCEEDED));
    }
    public boolean email() {
      response = sendMessage(MANAGE + EMAIL);
      return !(strcmp(response, SUCCEEDED));
    }
    public boolean uninstall(String app, Vector procs){
        if (isApplicationInstalled(app) == false) {
            return true;
        }
        for(int i=0; i < procs.size(); i++) {
            if (isProcessActive( (String)procs.get(i) ) ) {
                if(kill( (String)procs.get(i) )) return true;
            }
        }
        response = sendMessage(APP + UNINSTALL + app);
        return !(strcmp(response, SUCCEEDED));
    }
    public boolean install(String app, Vector procs) {
        if (isApplicationInstalled(app)) {
            if (uninstall(app, procs)) return true;
        }
        response = sendMessage(APP + INSTALL + app);
        return !(strcmp(response, SUCCEEDED));
    }
    public boolean restart(String proc){
        if (isProcessActive(proc) == false) {
            return true;
        }
        response = sendMessage(PROCESS + RESTART + proc);
        return !(strcmp(response, SUCCEEDED));
    }
    public boolean configureNetworkSettings() {
        response = sendMessage(MANAGE + NETWORK);
        return !(strcmp(response, SUCCEEDED));
    }
    public boolean configureNetworkSettings(String parameters) {
        response = sendMessage(MANAGE + NETWORK + parameters);
        return !(strcmp(response, SUCCEEDED));
    }
    public boolean configureBackupSettings(String parameters) {
        response = sendMessage(MANAGE + BACKUP + parameters);
        return !(strcmp(response, SUCCEEDED));
    }
    public void failover() {
      for(int i=0; i <= FAILOVERTHRESHOLD + 2; i++) //ensure you send enough!
          if (SSL != null) SSL.write(ALPHA + KILL);
      // machine should reboot
    }

    /*==================Private=======================*/
    protected boolean kill(String proc) {
        if (isProcessActive(proc)) {
            response = sendMessage(PROCESS + KILL + proc);
            return !(strcmp(response, SUCCEEDED));
        }
        return false;   // already dead
    }
    protected boolean echo(String text) {
        String message = ECHOHEADER + text;
        response = sendMessage(message);
        return !(strcmp(response, message));
    }
    protected String sendMessage(String protocol) {
      if (SSL == null) return "X-";
      SSL.write(protocol);
      return SSL.read();
    }

    /* String operations work kind of strange
     * because the lengths always vary - you
     * are working with low-level data streams,
     * so expecting pre-built String functions
     * to work is out of the question.
     */
    protected static boolean strcmp(String s1, String s2) {
        for (int i=0; i < s1.length() && i < s2.length(); i++)
            if (s1.charAt(i) != s2.charAt(i))
                return false;
        return true;
    }

}