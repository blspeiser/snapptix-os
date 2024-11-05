package Main;

import java.io.*;
import Snapptix_backend.*;

public class Test {
    public Test() {
        System.out.println("Starting tests...");
    }

    public static void SAMPLEmain(String[] args) {
        Test t = new Test();
        System.out.print("Declaring new API...");
        try {
            if(ConfirmBox.ask("Are you sure?") == false)
                throw new Exception("User did not want to!");
            API SnappCluster = new API();
            System.out.println("done.");
            if (SnappCluster.getMaxNodes() > 1) {
                // Cause a delay to allow heartbeat to configure
                for (int i=0; i < 1000; i++)
                    for (int j=0; j < 1000; j++)
                        for (int k=0; k < 100; k++)
                            System.out.print("");
                //End delay
                System.out.print("Heartbeat status: ");
                if ( SnappCluster.unitStatus.isAlpha() )
                    System.out.println("Alpha!");
                else
                    System.out.println("Beta.");
            }
            //System.out.print("Testing Mail engine...");
            //SnappCluster.mailer.send("This is a test!");
            //System.out.println("done.");

        } catch (Exception e) {
          System.out.println("failed: " + e);
        }
    }
}