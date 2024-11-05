package Snapptix_backend;

import java.io.*;
import java.util.*;

public abstract class AppModule {
    protected Janix  OSmanager = null;
    protected String packageName;
    protected String FriendlyName;
    protected Vector processNames;
    protected Vector configFileNames;
    
    protected String SPKpath = "/usr/share/snapptix/packages/";

    public AppModule(String pkgName, Janix client) {
        this.construct(pkgName, client);
    }

    public final boolean construct(String pkgName, Janix JanixClient) {
        this.packageName    = pkgName;
        this.OSmanager      = JanixClient;
        String s = new String("Did not read from file correctly.");
        StringTokenizer splitter;
        try {
            BufferedReader fso = new BufferedReader( 
                new FileReader(SPKpath + pkgName + ".spk")
            );
            s = fso.readLine();
            if (!s.equals(this.packageName) ) {
                System.out.println("Invalid pacakge format - " + packageName + " found " + s);
                System.exit(1);
            }
            this.FriendlyName = fso.readLine();
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the "["
            do {
                s = splitter.nextToken();
                if ( !s.equals("[") && !s.equals("]") )
                    processNames.add(s);
            } while ( ! s.equals("]") );
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the "["
            do {
                s = splitter.nextToken();
                if ( !s.equals("[") && !s.equals("]") )
                    configFileNames.add(s);
            } while ( ! s.equals("]") );
        } catch (Exception e) {
            System.out.println(e);
            return false;
        }
        return true;
    }
    
    public final boolean install() {
        try {
           if( OSmanager.isApplicationInstalled(this.packageName) ) {
               if (
                ConfirmBox.ask(this.FriendlyName + " is already installed. " +
                               "Do you want to reinstall it and overwrite current settings?") 
               )
                   return OSmanager.install(this.packageName, this.processNames);
               else
                   return false; // i.e. no error
           }
           return false; // i.e. no error
       } catch (Exception e) {
           e.printStackTrace();
           return true; // i.e. error
       }
    }
    
    public final boolean uninstall() {
        //inv: can only be called when installed!
        try {
            if (ConfirmBox.ask("Are you sure you want to uninstall " + this.FriendlyName + "?" ))
                return OSmanager.uninstall(this.packageName, this.processNames);
            else
                return false; // i.e. no error
        } catch (Exception e) {
                e.printStackTrace();
                return true; // i.e. error
        }
    }
    
    public abstract boolean loadSettings();
        // read in config settings from appropriate files 
    public abstract boolean saveSettings();
        // write to config files with appropriate settings
    public abstract void showWindow();
        // show configuration window
}
