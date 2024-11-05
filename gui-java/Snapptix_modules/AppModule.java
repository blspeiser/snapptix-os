package Snapptix_modules;

import java.io.*;
import java.util.*;
import Snapptix_utils.*;
import Snapptix_frontend.GLOBAL;
import Snapptix_backend.Janix;

public abstract class AppModule {
    protected Janix  OSmanager         = null;
    protected String packageName       = new String();
    protected String friendlyName      = new String();
    protected Vector processNames      = new Vector();
    protected Vector configFileNames   = new Vector();
    protected Vector logFileNames      = new Vector();
    protected javax.swing.JFrame owner = null;

    public final static String SPKpath = "/usr/share/snapptix/packages/";
//    public final static String SPKpath = "C:/";

    public AppModule(String pkgName) throws IOException {
        this.construct(pkgName, GLOBAL.SnapptixAPI.unixSystem, GLOBAL.App);
    }
    public AppModule() throws IOException {
       throw new IOException("No package name specified.");
    }

    public final String getPackageName()  { return this.packageName;  }
    public final String getFriendlyName() { return this.friendlyName; }

    public final void restart() {
        for(int i=0; i < this.processNames.size(); i++)
          this.OSmanager.restart( (String)this.processNames.get(i));
    }

    protected final boolean construct(String pkgName, Janix JanixClient,
     javax.swing.JFrame parent) throws IOException {
        this.packageName    = pkgName;
        this.OSmanager      = JanixClient;
        this.owner          = parent;
        String s = new String("Did not read from file correctly.");
        StringTokenizer splitter;
        try {
            BufferedReader fso = new BufferedReader(
                new FileReader(SPKpath + pkgName + ".spk")
            );
            s = fso.readLine();
            if (!s.equals(this.packageName) ) {
                throw new IOException("Invalid package format - " + packageName + " found " + s);
            }
            this.friendlyName = fso.readLine();
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the "["
            do {
                s = new String( splitter.nextToken() );
                if ( !s.equals("[") && !s.equals("]") )
                    processNames.add((String)s);
            } while ( ! s.equals("]") );
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the "["
            do {
                s = new String( splitter.nextToken() );
                if ( !s.equals("[") && !s.equals("]") )
                    configFileNames.add(s);
            } while ( ! s.equals("]") );
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the "["
            do {
                s = new String( splitter.nextToken() );
                if ( !s.equals("[") && !s.equals("]") )
                    logFileNames.add(s);
            } while ( ! s.equals("]") );
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public final boolean install() {
        try {
           if( OSmanager.isApplicationInstalled(this.packageName) ) {
               if (
                ConfirmBox.ask(this.friendlyName + " is already installed. " +
                               "Do you want to reinstall it and overwrite current settings?",
                               owner)
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
            if (ConfirmBox.ask("Are you sure you want to uninstall " + this.friendlyName + "?", owner))
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

    public static AppModule load(String moduleName) throws Exception{
      if (moduleName == null || moduleName.equalsIgnoreCase("") )
        throw new IOException("Invalid module name");
      return (AppModule)Class.forName("Snapptix_modules." + moduleName + "Module").newInstance();
    }
}
