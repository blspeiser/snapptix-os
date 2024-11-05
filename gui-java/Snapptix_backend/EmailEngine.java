package Snapptix_backend;

import java.io.*;
import Snapptix_frontend.GLOBAL;

public class EmailEngine {
    public String to       = new String("");
    public String from     = "SnappCluster Alert Service";
    public String subject  = "SnappCluster: Systems Alert";
    public boolean mailing = false;

    public EmailEngine() {
      this.load();
    }
    private final String email = "/usr/share/Janix/JavaEmail.mail";
    public void send(String messageBody)  {
      try {
         FileOutputStream fso = new FileOutputStream(email);
         String s = new String("");
         s += "FROM: "    + this.from    + "\n" +
              "TO: "      + this.to      + "\n" +
              "SUBJECT: " + this.subject + "\n" +
              messageBody;

         fso.write( s.getBytes() );
         fso.close();
         GLOBAL.SnapptixAPI.unixSystem.email(); //deletes the old email
      } catch (IOException e) {
         System.out.println(e);
      }
    }
    private final String emailSettings = "/usr/share/Snapptix/email.conf";
    public void save() {
      try {
         FileOutputStream fso = new FileOutputStream(emailSettings);
         String s = to + "\n";
         if(mailing)
           s+=1;
         else
           s+=0;
         fso.write( s.getBytes() );
         fso.close();
      } catch (IOException e) {
         System.out.println(e);
      }
    }
    public void load() {
      try {
        String s = new String("Did not read from file correctly.");
        BufferedReader fso = new BufferedReader( new FileReader(emailSettings) );
        to = fso.readLine();
        if (fso.readLine().equals("1"))
          mailing = true;
        else
          mailing = false;
        fso.close();
      } catch (Exception e) {
            to = "";
            mailing = false;
      }
    }

}
