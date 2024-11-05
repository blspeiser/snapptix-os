package Snapptix_backend;

import java.io.*;
import java.net.*;
import java.util.*;
import Snapptix_frontend.GLOBAL;


/* Heartbeat class
 * B. L. Speiser
 * Last made changes: 4/20/2003
 *
*/

public class Heartbeat extends Thread implements GlobalIP {
    protected final int SLEEPTIME       = 5000;     // milliseconds
    protected final int WAITTIME        = 250;      // milliseconds
    protected final int SOCKETTIMEOUT   = 50;       // milliseconds

    String          status  = null;
    ServerSocket    listen  = null;
    Socket          sd      = null;
    DataOutputStream out    = null;
    BufferedReader in       = null;

    String thisHost, otherHost;
    int myRank, otherRank;
    int unitNumber, otherUnitNumber;
    int noUpdateCount;
    int BasePort = 7890;

    boolean connected = false;

/************************PUBLIC INTERFACE *********************************/
    public Heartbeat(int unit) {
        myRank          = 0;
        otherRank       = 0;
        noUpdateCount   = 0;
        if (unit % 2 == 0)
            this.unitNumber = 2;
        else
            this.unitNumber = 1;
        if (unitNumber == 1)
            otherUnitNumber = 2;
        else
            otherUnitNumber = 1;
        thisHost = BaseIP + unitNumber; otherHost = BaseIP + otherUnitNumber;
        status = BETAUNIT;
        try {
            listen = new ServerSocket(BasePort + unitNumber, 5); // backlog
            listen.setSoTimeout(SOCKETTIMEOUT * 5); // ensures a request should come in that timespan
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    public boolean isAlpha()    { return strcmp(status, ALPHAUNIT); }
    public boolean isBeta()     { return strcmp(status, BETAUNIT ); }
    public boolean isOtherUnitConnected()  { return connected;      }

    public String getStatus()  { return status; }
    public  void  setStatus(String s) {
        try {
            this.status = s;
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    public int getThisRank()  { return this.myRank;    }
    public int getOtherRank() { return this.otherRank; }

/************************END PUBLIC INTERFACE *********************************/
    public Heartbeat(int unit, String me, String him) {
        thisHost = me; otherHost = him;
        myRank          = 0;
        otherRank       = 0;
        noUpdateCount   = 0;
        if (unit % 2 == 0)
            this.unitNumber = 2;
        else
            this.unitNumber = 1;
        if (unitNumber == 1)
            otherUnitNumber = 2;
        else
            otherUnitNumber = 1;
        status = BETAUNIT;
        try {
            listen = new ServerSocket(BasePort + unitNumber, 5); // backlog
            listen.setSoTimeout(SOCKETTIMEOUT * 5); // ensures a request should come in that timespan
        } catch (Exception e) {
            System.out.println(e);
        }
    }
    private boolean wasBeta = false;
    public void run() {
        synchronize();
        if (myRank > otherRank)
            alphaContest();
        // All branches from here on end up back inside this loop!
        while (true) {
          try{
            if ( this.isBeta() ) wasBeta = true; else wasBeta = false;
            this.sleep(SLEEPTIME);
            synchronize();
            if (strcmp(status, ALPHAUNIT)) {
                if (myRank > otherRank) {
                    if (otherRank == 0)
                        myRank = 1;
                    else
                        alphaContest();
                }
            } else { // i.e. Beta Unit
                if (otherRank > myRank)
                    myRank = 0;
                else
                    alphaContest();
            }
            if (this.isAlpha() && wasBeta &&
                GLOBAL.SnapptixAPI.mailer != null && GLOBAL.SnapptixAPI.mailer.mailing)
                GLOBAL.SnapptixAPI.mailer.send("Unit " + this.unitNumber + " promoted.  Check other unit." );
          } catch(Exception e) {
              e.printStackTrace();
          }
         } // end while
    }

    protected void alphaContest() {
        myRank += 2;
        synchronize();
        while(myRank == otherRank) {
            Random rand = new Random();
            try {
                this.sleep( rand.nextInt(WAITTIME) );
            } catch (Exception e) {
                System.out.println(e);
            }
            synchronize();
            if (myRank < otherRank)
                break;
            else
                myRank += 2;
            synchronize();
        }
        if (myRank > otherRank)
            status = ALPHAUNIT;
        else
            status = BETAUNIT;
    }

    protected void synchronize() {
        try {
          sd = listen.accept();
          this.in  = new BufferedReader( new InputStreamReader( sd.getInputStream() ) );
          String reply = new String( in.readLine() );
          otherRank = Integer.parseInt(reply);
          in.close();
          sd.close();
          connected = true;
         } catch(SocketTimeoutException noOtherUnit) {
            otherRank = 0;
            connected = false;
        } catch(ConnectException noOtherUnit) {
            otherRank = 0;
            connected = false;
        } catch (IOException e) {
            System.out.println(e);
        } catch(Exception e) {
            System.out.println(e);
        }
        try {
          sd = new Socket();
          sd.connect( new InetSocketAddress(otherHost, BasePort + otherUnitNumber), SOCKETTIMEOUT);
          this.out = new DataOutputStream(sd.getOutputStream());
          out.write((Integer.toString(myRank)).getBytes());
          out.flush();
          out.close();
          sd.close();
          connected = true;
        } catch(SocketTimeoutException noOtherUnit) {
            otherRank = 0;
            connected = false;
        } catch(ConnectException noOtherUnit) {
            otherRank = 0;
            connected = false;
        } catch (IOException e) {
            System.out.println(e);
        } catch(Exception e) {
            System.out.println(e);
        }
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



