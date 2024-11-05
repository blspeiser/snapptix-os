package Snapptix_backend;

import java.io.*;
import java.util.*;

public class NetworkSettings {
    protected String FILEPATH = "/etc/snapptix.conf";

    public String thisHostName;
    public String thisIP;
    public String thisEthInterface;
    public String thisSubnet;
    public String thisGateway;
    public String thisPrimaryDNS;
    public String thisSecondaryDNS;

    public String otherHostName;
    public String otherIP;
    public String otherEthInterface;
    public String otherSubnet;
    public String otherGateway;
    public String otherPrimaryDNS;
    public String otherSecondaryDNS;

    public String ClusterName;
    public String ClusterIP;

    private int unitNum;


    public NetworkSettings(int unit) {
        this.unitNum = unit;
    }

    public NetworkSettings(int unit, String configFile) {
        this.unitNum = unit;
        this.FILEPATH = configFile;
    }

    public int getThisUnitNum() { return this.unitNum; }
    public int getOtherUnitNum() { return (this.unitNum == 1 ? 2 : 1); }

    public static String ip(int oct1, int oct2, int oct3, int oct4) {
        String s = new String("");
        s += oct1 + "." + oct2 + "." + oct3 + "." + oct4;
        return s;
    }

    // Refer to RFC 2 for file format
    public String toString() {
        String s = new String("");
        if (unitNum == 1) {
            s += "1 " + thisHostName  + " " + thisIP  + " " + thisEthInterface  + "\n";
            s += "2 " + otherHostName + " " + otherIP + " " + otherEthInterface + "\n";
            s += "P " + ClusterName   + " " + ClusterIP     + "\n";
            s += "S " + thisSubnet    + " " + otherSubnet   + "\n";
            s += "G " + thisGateway   + " " + otherGateway  + "\n";
            s += "DNS1 " + thisPrimaryDNS   + " " + otherPrimaryDNS   + "\n";
            s += "DNS2 " + thisSecondaryDNS + " " + otherSecondaryDNS + "\n";
        } else { // i.e. unitNum := 2
            s += "1 " + otherHostName + " " + otherIP + " " + otherEthInterface + "\n";
            s += "2 " + thisHostName  + " " + thisIP  + " " + thisEthInterface  + "\n";
            s += "P " + ClusterName   + " " + ClusterIP     + "\n";
            s += "S " + otherSubnet   + " " + thisSubnet    + "\n";
            s += "G " + otherGateway  + " " + thisGateway   + "\n";
            s += "DNS1 " + otherPrimaryDNS   + " " + thisPrimaryDNS   + "\n";
            s += "DNS2 " + otherSecondaryDNS + " " + thisSecondaryDNS + "\n";
        }
        return s;
    }



    public void save() {
        String s = this.toString();
        //System.out.println(s);
        try {
            FileOutputStream fso = new FileOutputStream(FILEPATH);
            fso.write(s.getBytes());
            fso.close();
        } catch (IOException e) {
            System.out.println(e);
        }
    }

    public void load() {
        String s = new String("Did not read from file correctly.");
        StringTokenizer splitter;
        try {
            BufferedReader fso = new BufferedReader(
                new FileReader(FILEPATH)
            );
            if (unitNum == 1) {
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the number
                thisHostName        = splitter.nextToken();
                thisIP              = splitter.nextToken();
                thisEthInterface    = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the number
                otherHostName        = splitter.nextToken();
                otherIP              = splitter.nextToken();
                otherEthInterface    = splitter.nextToken();
            } else { // unitNum := 2
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the number
                otherHostName        = splitter.nextToken();
                otherIP              = splitter.nextToken();
                otherEthInterface    = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the number
                thisHostName         = splitter.nextToken();
                thisIP               = splitter.nextToken();
                thisEthInterface     = splitter.nextToken();
            }
            splitter = new StringTokenizer(fso.readLine());
            splitter.nextToken(); // skip the "P "
            ClusterName = splitter.nextToken();
            ClusterIP   = splitter.nextToken();
            splitter = new StringTokenizer(fso.readLine());
            if (unitNum == 1) {
                splitter.nextToken(); // skip the "S "
                thisSubnet  = splitter.nextToken();
                otherSubnet = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the "G "
                thisGateway  = splitter.nextToken();
                otherGateway = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the "DNS1 "
                thisPrimaryDNS  = splitter.nextToken();
                otherPrimaryDNS = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the "DNS2 "
                thisSecondaryDNS  = splitter.nextToken();
                otherSecondaryDNS = splitter.nextToken();
            } else { // i.e. unitNum := 2
                splitter.nextToken(); // skip the "S "
                otherSubnet = splitter.nextToken();
                thisSubnet  = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the "G "
                otherGateway = splitter.nextToken();
                thisGateway  = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the "DNS1 "
                otherPrimaryDNS = splitter.nextToken();
                thisPrimaryDNS  = splitter.nextToken();
                splitter = new StringTokenizer(fso.readLine());
                splitter.nextToken(); // skip the "DNS2 "
                otherSecondaryDNS = splitter.nextToken();
                thisSecondaryDNS  = splitter.nextToken();
            }
            fso.close();
        } catch (Exception e) {
            System.out.println(e);
        }
    }
}
