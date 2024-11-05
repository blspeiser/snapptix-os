package Snapptix_backend;

import java.io.*;
import java.util.*;
import Snapptix_frontend.GLOBAL;
import Snapptix_modules.AppModule;

public class API {
    //private boolean DEBUG = true; // for Win32
    private boolean DEBUG = false;  // for Linux

    public Janix            unixSystem           = null;
    public Heartbeat        unitStatus           = null;
    public NetworkSettings  networkConfiguration = null;
    public EmailEngine      mailer               = null;

    public final String clusterConfigFile    = "/etc/node.conf";
    public final String cpuStats             = "/proc/loadavg";
    public final String memoryStats          = "/proc/meminfo";
    public final String networkStats         = "/proc/net/dev";
    public final String applog               = "/usr/share/Janix/apps.log";
    public final String LOGS                 = "/var/log/";

    protected int maxnodes = 0;

    public int getMaxNodes() {
        return this.maxnodes;
    }

    public boolean isAlpha() {
      if (maxnodes < 2) return true;
      else return this.unitStatus.isAlpha();
    }
    public boolean isBeta() {
      if (maxnodes < 2) return false;
      else return this.unitStatus.isBeta();
    }

    public API() {
        String s = new String(clusterConfigFile + " not found.");
        StringTokenizer splitter;
        int unit = 0;
        try {
            BufferedReader fso = new BufferedReader(
                (DEBUG ? new FileReader("c:/node.txt") : new FileReader(clusterConfigFile) )
            );
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the MAXNODES header
            s = splitter.nextToken(); // grab max units
            maxnodes = Integer.parseInt(s);
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the NODE header
            s = splitter.nextToken(); // grab max units
            unit = Integer.parseInt(s);
            fso.close();
        } catch (Exception e) {
            System.out.println(e);
            if(unit != 1 && unit != 2)
                System.exit(1);
        }
        networkConfiguration = (DEBUG ? new NetworkSettings(unit, "C:/settings.txt") :
                                new NetworkSettings(unit)  // keeps track of unit
        );
        networkConfiguration.load();
        unixSystem = (DEBUG ? new Janix(0) : new Janix() );
        mailer = new EmailEngine();
        if (maxnodes > 1) {
          /** @todo:  SET 217.X INTERFACE! */
          // When using 192.168.217.x interfaces, use this constructor:
            unitStatus = new Heartbeat( this.networkConfiguration.getThisUnitNum() );
          //When using your own IP addresses, use this constructor:
//            unitStatus = new Heartbeat( this.networkConfiguration.getThisUnitNum(), this.networkConfiguration.thisHostName, this.networkConfiguration.otherHostName);
            unitStatus.setDaemon(true); // Heartbeat is terminated on API's closure
            unitStatus.start(); // ie, NOT run() !
        }

        try {
          BufferedReader fso = new BufferedReader(
                (DEBUG ? new FileReader("c:/node.txt") : new FileReader(clusterConfigFile) )
            );
        } catch(IOException e) {
          e.printStackTrace();
        }
    }

  public String getMemoryStatistics() {
    try {
      FileInputStream FS = new FileInputStream( (DEBUG ? "c:/mem.txt" : memoryStats ) );
      BufferedReader BR = new BufferedReader(new FileReader(FS.getFD()));
      for(int i = 0; i<3; i++)
        BR.readLine();
      String total = BR.readLine();
      String free  = BR.readLine();
      StringTokenizer splitter  = new StringTokenizer(total);
      splitter.nextToken(); // Skip header
      total = splitter.nextToken();
      splitter  = new StringTokenizer(free);
      splitter.nextToken(); // Skip header
      free = splitter.nextToken();
      int f = Integer.parseInt(free);
      int t = Integer.parseInt(total);
      BR.close();
      return  (100*f)/t + "%";
    } catch (Exception e) {
      e.printStackTrace();
      return new String("0%");
    }
  }
  public String getCPUStatistics() {
    try {
      FileInputStream FS = new FileInputStream( (DEBUG ? "c:/mem.txt" : cpuStats ) );
      BufferedReader BR = new BufferedReader(new FileReader(FS.getFD()));
      StringTokenizer splitter  = new StringTokenizer( BR.readLine() );
      BR.close();
      String  load[] = new String[3];
      int i;
      for(i=0; i<3; i++) load[i] = splitter.nextToken();
      int percentages[] = new int[3];
      int sum = 0;
      for(i=0; i<3; i++) {
        splitter = new StringTokenizer( load[i], "." );
        if (splitter.nextToken().equals("1"))
          percentages[i] = 100;
        else
          percentages[i] = Integer.parseInt( splitter.nextToken() );
        sum += percentages[i];
      }
      sum /= 3; //Take average
      return sum + "%";
    } catch (Exception e) {
      e.printStackTrace();
      return new String("0%");
    }
  }
  public String getNumberOfProcesses() {
      try {
        FileInputStream FS = new FileInputStream( (DEBUG ? "c:/mem.txt" : cpuStats ) );
        BufferedReader BR = new BufferedReader(new FileReader(FS.getFD()));
        StringTokenizer splitter  = new StringTokenizer( BR.readLine(), "/" );
        BR.close();
        splitter.nextToken(); //ignore CPU stats on left
        splitter = new StringTokenizer( splitter.nextToken() );
        return splitter.nextToken();
      } catch (Exception e) {
        e.printStackTrace();
        return new String("0");
      }
  }

  private long received=0, transmitted=0;
  public String getNetworkThroughput() {
    String s;
      try {
        FileInputStream FS = new FileInputStream( (DEBUG ? "c:/net.txt" : networkStats ) );
        BufferedReader BR = new BufferedReader(new FileReader(FS.getFD()));
        for (int i=0; i<3; i++) BR.readLine(); // skip first few lines header
        StringTokenizer splitter  = new StringTokenizer( BR.readLine(), ":" );
        while((s = splitter.nextToken()).equals(this.networkConfiguration.thisEthInterface)) {
          // Go to next line
           splitter = new StringTokenizer( BR.readLine(), ":" );
        }
        splitter = new StringTokenizer( splitter.nextToken() );
        long r = Integer.parseInt( s=splitter.nextToken() );
        for(int i=0; i<7; i++) splitter.nextToken();
        long t = Integer.parseInt( splitter.nextToken() );
        BR.close();
        // differential equation:
        long rate = 1000*((r - received) + (t - transmitted))/GLOBAL.guiREFRESHTIME; //milliseconds
        // rate is in bytes
        rate /= 1024;
        //before exiting, set "old" rates
        this.received    = r;
        this.transmitted = t;
        return (rate + " Kbps");
      } catch (Exception e) {
        return new String("ERROR");
      }
  }
  public boolean isPassword(String s) {
    return !unixSystem.changePassword(s,s);
    // should return false (no error) if it succeeds in writing the "new" password,
    // because if the password is wrong then it won't write the new one anyway
  }
  public boolean setPassword(String oldpw, String newpw) {
    //return true on error
     if (isPassword(oldpw))
       return unixSystem.changePassword(oldpw,newpw);
     else
       return true;
  }
  public Vector getModules() {
    Vector v = new Vector();
    try{
      FileInputStream FS = new FileInputStream( (DEBUG ? "c:/applist.txt" : applog ) );
      BufferedReader BR = new BufferedReader(new FileReader(FS.getFD()));
      String s;
      while ( (s = BR.readLine()) != null && !s.equalsIgnoreCase("") ) {
        v.add( AppModule.load(s)  );
      }
      return v;
    } catch (Exception e) {
      e.printStackTrace();
      return v;
    }
  }
}