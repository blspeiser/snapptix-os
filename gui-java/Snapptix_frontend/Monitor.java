package Snapptix_frontend;

import java.io.*;
import java.util.*;

public class Monitor extends TimerTask implements GLOBAL {

  public Monitor() {
  }

  public void run(){
  //Set Memory Statistics
    sStats.setMem( SnapptixAPI.getMemoryStatistics() );
  //Set CPU Statistics
    sStats.setCPU( SnapptixAPI.getCPUStatistics() );
  //Set Processes Statistics
    sStats.setProcs( SnapptixAPI.getNumberOfProcesses() );
  //Set Network stats
    sStats.setThroughput( SnapptixAPI.getNetworkThroughput() );

    if (SnapptixAPI.isBeta() ) {
      // Check applist and reload screen
      Vector currentApps = SnapptixAPI.getModules();
      if(!currentApps.equals(App.modules)) { // changes were made!
        App.resetApplist();
        App.PT.resetList();
        App.modules = currentApps;
        App.populateScreen();
        if(App.jTabbedPane1.indexOfTab("Applications") ==
           App.jList_Application.getSelectedIndex()     )
              App.repaint();
      }
    }

    if ( SnapptixAPI.getMaxNodes() < 2) {
      sStats.repaint();
      return; // all done
    }
    //otherwise....
    if ( SnapptixAPI.isAlpha() ) {
      sStats.setRole(AlphaUnit);
      App.setClusterSettings(true);
      App.setTheseSettings(true);
      App.setOtherSettings(true);
    }
    else {
      sStats.setRole(BetaUnit);
      App.setClusterSettings(false);
      App.setTheseSettings(false);
      App.setOtherSettings(false);
   }
  //Set Heartbeat statistics
    // set this machine first:  1 is normal for Alpha, 0 is normal for beta
    char me, him;
    if( SnapptixAPI.unitStatus.getThisRank() == (SnapptixAPI.isAlpha() ? 1 : 0) )
      me = 'G';
    else
      me = 'Y'; // (abnormal)
    // establish current connection status and set alternate
    if ( SnapptixAPI.unitStatus.isOtherUnitConnected() ) {
      Light.addHeartbeat();
      // if I'm Alpha, then other unit should be beta; meaning the other should be 0
      if( SnapptixAPI.unitStatus.getOtherRank() == (SnapptixAPI.isAlpha() ? 0 : 1) )
        him = 'G';
      else
        him = 'Y'; //(abnormal)
    } else { // Not connected!
      if ( SnapptixAPI.isBeta() )
        me =  'Y'; //should be promoting
      Light.killHeartbeat();
      him = 'R'; // it's down!11
    }
    Light.set(SnapptixAPI.networkConfiguration.getThisUnitNum(),  me );
    Light.set(SnapptixAPI.networkConfiguration.getOtherUnitNum(), him);
    sStats.repaint();
  } // END MONITORING
}

