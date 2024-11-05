package Snapptix_frontend;

import java.awt.*;
import java.awt.event.*;
import java.net.*;
import javax.swing.*;
import com.borland.jbcl.layout.*;
import java.util.*;
import java.beans.*;
import Snapptix_modules.*;
import Snapptix_frontend.*;
import Snapptix_utils.*;
import com.borland.internetbeans.*;
import sun.java2d.pipe.*;


public class AppFrame extends JFrame implements GLOBAL {
  /** @todo Beginning of AppFrame */
  JPanel          contentPane;
  XYLayout        xYLayout1 = new XYLayout();
  XYLayout        xYLayout2 = new XYLayout();
  Vector          applist = new Vector();
  JLabel          StatMesgs = new JLabel();
  // NEED TO USE POINTER REFERENCES TO USE THESE IN DESIGN
  BEAN_TrafficLight     LightPtr  = Light;
  BEAN_ServerStatistics sStatsPtr = sStats;
  Vector RunningServiceNames = new Vector(1,1);
  Vector modules = new Vector();

  JTabbedPane jTabbedPane1 = new JTabbedPane();
  JPanel jPanel_Status      = new JPanel();
  JPanel jPanel_Network     = new JPanel();
  JPanel jPanel_Application = new JPanel();
  JPanel jPanel_System      = new JPanel();

  XYLayout xYSystemLayout = new XYLayout();
  XYLayout xYApplicationsLayout = new XYLayout();
  IPverifier ipv = new IPverifier(this);

/** @todo Declaring Network Tab JComponents */
  //cluster JComponents
  private JLabel     clusterNameLabel      = new JLabel();
  private JTextField clusterNameValue      = new JTextField();
  private JLabel     clusterIPAddrLabel    = new JLabel();
  private JTextField clusterIPAddrValue[]  = { new JTextField(),
                                               new JTextField(),
                                               new JTextField(),
                                               new JTextField()};
  // By row - Row 1:  Machine names
  private JLabel     machineNameLabel       = new JLabel();
  private JTextField thisMachineNameValue  = new JTextField();
  private JTextField otherMachineNameValue   = new JTextField();

  // Row 2: Node IP addresses
  private JLabel     machIPAddrLabel    = new JLabel();
  private JTextField thisIPAddrValue[] = { new JTextField(),
                                            new JTextField(),
                                            new JTextField(),
                                            new JTextField()};
  private JTextField otherIPAddrValue[]  = { new JTextField(),
                                            new JTextField(),
                                            new JTextField(),
                                            new JTextField()};
  // Row 3: Subnet Masks
  private JLabel     subnetIPAddrLabel        = new JLabel();
  private JTextField thisSubnetIPAddrValue[] = { new JTextField(),
                                                  new JTextField(),
                                                  new JTextField(),
                                                  new JTextField()};
  private JTextField otherSubnetIPAddrValue[]  = { new JTextField(),
                                                  new JTextField(),
                                                  new JTextField(),
                                                  new JTextField()};
  // Row 4: Gateways
  private JLabel     gatewayIPAddrLabel         = new JLabel();
  private JTextField thisGatewayIPAddrValue[]  = { new JTextField(),
                                                    new JTextField(),
                                                    new JTextField(),
                                                    new JTextField()};
  private JTextField otherGatewayIPAddrValue[]  = { new JTextField(),
                                                   new JTextField(),
                                                   new JTextField(),
                                                   new JTextField()};
  // Row 5: Primary DNS
  private JLabel     primaryDNSLabel         = new JLabel();
  private JTextField thisPrimaryDNSValue[]  = { new JTextField(),
                                                 new JTextField(),
                                                 new JTextField(),
                                                 new JTextField()};
  private JTextField otherPrimaryDNSValue[]  = { new JTextField(),
                                                new JTextField(),
                                                new JTextField(),
                                                new JTextField()};
  // Row 6: Alternate DNS
  private JLabel altDNSLabel                 = new JLabel();
  private JTextField thisSecondaryDNSValue[]  = { new JTextField(),
                                                   new JTextField(),
                                                   new JTextField(),
                                                   new JTextField()};
  private JTextField otherSecondaryDNSValue[]  = { new JTextField(),
                                                  new JTextField(),
                                                  new JTextField(),
                                                  new JTextField()};
  // literal periods between nodes of ip addrs - row + col format
  private JLabel period3_6 = new JLabel();
  private JLabel period1_1 = new JLabel();
  private JLabel period3_2 = new JLabel();
  private JLabel period3_1 = new JLabel();
  private JLabel period3_3 = new JLabel();
  private JLabel period2_1 = new JLabel();
  private JLabel period2_2 = new JLabel();
  private JLabel period2_3 = new JLabel();
  private JLabel period4_1 = new JLabel();
  private JLabel period4_3 = new JLabel();
  private JLabel period4_2 = new JLabel();
  private JLabel period5_3 = new JLabel();
  private JLabel period5_2 = new JLabel();
  private JLabel period5_1 = new JLabel();
  private JLabel period3_4 = new JLabel();
  private JLabel period3_5 = new JLabel();
  private JLabel period5_5 = new JLabel();
  private JLabel period5_6 = new JLabel();
  private JLabel period5_4 = new JLabel();
  private JLabel period4_6 = new JLabel();
  private JLabel period4_5 = new JLabel();
  private JLabel period4_4 = new JLabel();
  private JLabel period2_5 = new JLabel();
  private JLabel period2_6 = new JLabel();
  private JLabel period2_4 = new JLabel();
  private JLabel periodC_1 = new JLabel();
  private JLabel period1_2 = new JLabel();
  private JLabel period1_3 = new JLabel();
  private JLabel period1_4 = new JLabel();
  private JLabel period1_5 = new JLabel();
  private JLabel period1_6 = new JLabel();
  private JLabel periodC_2 = new JLabel();
  private JLabel periodC_3 = new JLabel();

  private JLabel thisLabel = new JLabel();
  private JLabel otherLabel  = new JLabel();

  XYLayout xYNetworkLayout = new XYLayout();

  JButton networkApplyButton  = new JButton();
  JButton networkCancelButton = new JButton();

/** @todo Declaring Application Tab JComponents */
  JList jList_Application               = new JList();
  JButton applicationsInstallButton     = new JButton();
  JButton applicationsConfigureButton   = new JButton();
  JButton applicationsUninstallButton   = new JButton();

/** @todo Declaring Program Tree */
  ProgramTree jTree1                    = PT;
  JLabel servicesLabel                  = new JLabel();
  ButtonGroup buttonGroup_ShareType     = new ButtonGroup();
  ButtonGroup buttonGroup1              = new ButtonGroup();
  ButtonGroup buttonGroup_ActionType    = new ButtonGroup();

/** @todo Decaring Monitor */
  Monitor SysMon = MT;
  java.util.Timer TM = new java.util.Timer();

/** @todo Declaring System Tab JComponents */
  private JButton systemChgPwdButton                = new JButton();
  private JButton systemEmailAlterSettingsButton1   = new JButton();
  private JButton systemLockSystemButton1           = new JButton();
  private JButton systemViewLogsButton1             = new JButton();
  private JButton systemShutdownSnappClusterButton  = new JButton();
  private JButton systemTestFailoverButton          = new JButton();
  private JButton systemBackupButton                = new JButton();
  private JToggleButton mountFloppyToggle = new JToggleButton();
  private JToggleButton mountCdToggle = new JToggleButton();



  /** @todo CONSTRUCTOR */
  public AppFrame() {
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }

  /** @todo Component Initialization*/
  private void jbInit() throws Exception  {
    contentPane = (JPanel) this.getContentPane();
    contentPane.setLayout(xYLayout1);
    this.setResizable(false);
    this.setTitle("SnappCluster");
    servicesLabel.setText("Services:");
    Dimension d = new Dimension(800,600);
    this.setSize(d);
    contentPane.setMinimumSize(d);
    contentPane.setPreferredSize(d);

    //Panels:
    jPanel_Status.setLayout(xYLayout2);
    jPanel_System.setLayout(xYSystemLayout);
    jPanel_Application.setLayout(xYApplicationsLayout);
    jPanel_Network.setLayout(xYNetworkLayout);

    /** @todo Setting up Network Panel */
    machIPAddrLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    machIPAddrLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    machIPAddrLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    machineNameLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    machineNameLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    machineNameLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    machineNameLabel.setText("Machine name:");
    machIPAddrLabel.setText("Machine IP:");
    primaryDNSLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    primaryDNSLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    primaryDNSLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    primaryDNSLabel.setText("Primary DNS:");
    subnetIPAddrLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    subnetIPAddrLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    subnetIPAddrLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    subnetIPAddrLabel.setText("Subnet:");
    period1_1.setFont(new java.awt.Font("Dialog", 1, 12));
    period1_1.setHorizontalAlignment(SwingConstants.CENTER);
    period1_1.setText(".");
    period2_1.setText(".");
    period2_1.setHorizontalAlignment(SwingConstants.CENTER);
    period2_1.setFont(new java.awt.Font("Dialog", 1, 12));
    period4_1.setFont(new java.awt.Font("Dialog", 1, 12));
    period4_1.setHorizontalAlignment(SwingConstants.CENTER);
    period4_1.setText(".");
    period2_2.setFont(new java.awt.Font("Dialog", 1, 12));
    period2_2.setHorizontalAlignment(SwingConstants.CENTER);
    period2_2.setText(".");
    period4_2.setFont(new java.awt.Font("Dialog", 1, 12));
    period4_2.setHorizontalAlignment(SwingConstants.CENTER);
    period4_2.setText(".");
    period2_3.setFont(new java.awt.Font("Dialog", 1, 12));
    period2_3.setHorizontalAlignment(SwingConstants.CENTER);
    period2_3.setText(".");
    period4_3.setFont(new java.awt.Font("Dialog", 1, 12));
    period4_3.setHorizontalAlignment(SwingConstants.CENTER);
    period4_3.setText(".");
    period3_2.setText(".");
    period3_2.setHorizontalAlignment(SwingConstants.CENTER);
    period3_2.setFont(new java.awt.Font("Dialog", 1, 12));
    gatewayIPAddrLabel.setText("Gateway IP:");
    gatewayIPAddrLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    gatewayIPAddrLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    gatewayIPAddrLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    period3_1.setText(".");
    period3_1.setHorizontalAlignment(SwingConstants.CENTER);
    period3_1.setFont(new java.awt.Font("Dialog", 1, 12));
    period3_3.setText(".");
    period3_3.setHorizontalAlignment(SwingConstants.CENTER);
    period3_3.setFont(new java.awt.Font("Dialog", 1, 12));
    period5_3.setText(".");
    period5_3.setHorizontalAlignment(SwingConstants.CENTER);
    period5_3.setFont(new java.awt.Font("Dialog", 1, 12));
    period5_2.setText(".");
    period5_2.setHorizontalAlignment(SwingConstants.CENTER);
    period5_2.setFont(new java.awt.Font("Dialog", 1, 12));
    period5_1.setText(".");
    period5_1.setHorizontalAlignment(SwingConstants.CENTER);
    period5_1.setFont(new java.awt.Font("Dialog", 1, 12));
    altDNSLabel.setText("Secondary DNS:");
    altDNSLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    altDNSLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    altDNSLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    period3_6.setFont(new java.awt.Font("Dialog", 1, 12));
    period3_6.setHorizontalAlignment(SwingConstants.CENTER);
    period3_6.setText(".");
    period3_4.setFont(new java.awt.Font("Dialog", 1, 12));
    period3_4.setHorizontalAlignment(SwingConstants.CENTER);
    period3_4.setText(".");
    period3_5.setFont(new java.awt.Font("Dialog", 1, 12));
    period3_5.setHorizontalAlignment(SwingConstants.CENTER);
    period3_5.setText(".");
    period5_5.setFont(new java.awt.Font("Dialog", 1, 12));
    period5_5.setHorizontalAlignment(SwingConstants.CENTER);
    period5_5.setText(".");
    period5_6.setFont(new java.awt.Font("Dialog", 1, 12));
    period5_6.setHorizontalAlignment(SwingConstants.CENTER);
    period5_6.setText(".");
    period5_4.setFont(new java.awt.Font("Dialog", 1, 12));
    period5_4.setHorizontalAlignment(SwingConstants.CENTER);
    period5_4.setText(".");
    period4_6.setText(".");
    period4_6.setHorizontalAlignment(SwingConstants.CENTER);
    period4_6.setFont(new java.awt.Font("Dialog", 1, 12));
    period4_5.setText(".");
    period4_5.setHorizontalAlignment(SwingConstants.CENTER);
    period4_5.setFont(new java.awt.Font("Dialog", 1, 12));
    period4_4.setText(".");
    period4_4.setHorizontalAlignment(SwingConstants.CENTER);
    period4_4.setFont(new java.awt.Font("Dialog", 1, 12));
    period2_6.setText(".");
    period2_6.setHorizontalAlignment(SwingConstants.CENTER);
    period2_6.setFont(new java.awt.Font("Dialog", 1, 12));
    period2_5.setText(".");
    period2_5.setHorizontalAlignment(SwingConstants.CENTER);
    period2_5.setFont(new java.awt.Font("Dialog", 1, 12));
    period2_4.setFont(new java.awt.Font("Dialog", 1, 12));
    period2_4.setHorizontalAlignment(SwingConstants.CENTER);
    period2_4.setText(".");
    clusterNameLabel.setText("Cluster name:");
    clusterNameLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    clusterNameLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    clusterNameLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    clusterIPAddrLabel.setText("Cluster IP:");
    clusterIPAddrLabel.setHorizontalTextPosition(SwingConstants.RIGHT);
    clusterIPAddrLabel.setHorizontalAlignment(SwingConstants.RIGHT);
    clusterIPAddrLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    periodC_1.setText(".");
    periodC_1.setHorizontalAlignment(SwingConstants.CENTER);
    periodC_1.setFont(new java.awt.Font("Dialog", 1, 12));
    period1_2.setText(".");
    period1_2.setHorizontalAlignment(SwingConstants.CENTER);
    period1_2.setFont(new java.awt.Font("Dialog", 1, 12));
    period1_3.setFont(new java.awt.Font("Dialog", 1, 12));
    period1_3.setHorizontalAlignment(SwingConstants.CENTER);
    period1_3.setText(".");
    period1_4.setText(".");
    period1_4.setHorizontalAlignment(SwingConstants.CENTER);
    period1_4.setFont(new java.awt.Font("Dialog", 1, 12));
    period1_5.setText(".");
    period1_5.setHorizontalAlignment(SwingConstants.CENTER);
    period1_5.setFont(new java.awt.Font("Dialog", 1, 12));
    period1_6.setText(".");
    period1_6.setHorizontalAlignment(SwingConstants.CENTER);
    period1_6.setFont(new java.awt.Font("Dialog", 1, 12));
    periodC_2.setFont(new java.awt.Font("Dialog", 1, 12));
    periodC_2.setHorizontalAlignment(SwingConstants.CENTER);
    periodC_2.setText(".");
    periodC_3.setText(".");
    periodC_3.setHorizontalAlignment(SwingConstants.CENTER);
    periodC_3.setFont(new java.awt.Font("Dialog", 1, 12));
    thisLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    thisLabel.setHorizontalAlignment(SwingConstants.CENTER);
    otherLabel.setHorizontalAlignment(SwingConstants.CENTER);
    otherLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    thisLabel.setText("This Computer (Unit " + SnapptixAPI.networkConfiguration.getThisUnitNum() + ")");
    otherLabel.setText("Other Computer (Unit " + SnapptixAPI.networkConfiguration.getOtherUnitNum() + ")");
    networkApplyButton.setText("Apply");
    networkApplyButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        networkApplyButton_actionPerformed(e);
      }
    });
    networkCancelButton.setText("Cancel");
    networkCancelButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        networkCancelButton_actionPerformed(e);
      }
    });
    /** @todo Setting up Application Panel */
    applicationsInstallButton.setText("Install");
    applicationsInstallButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        applicationsInstallButton_actionPerformed(e);
      }
    });
    applicationsConfigureButton.setText("Configure");
    applicationsConfigureButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        applicationsConfigureButton_actionPerformed(e);
      }
    });
    applicationsUninstallButton.setText("Uninstall");
    applicationsUninstallButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        applicationsUninstallButton_actionPerformed(e);
      }
    });
    /** @todo Add services label and set window size */
    servicesLabel.setText("Services:");
    contentPane.setMinimumSize(new Dimension(800, 600));
    contentPane.setPreferredSize(new Dimension(800, 600));
    /** @todo Setting up System Tab JComponents */
    systemChgPwdButton.setText("Change Password");
    systemChgPwdButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        systemChgPwdButton_actionPerformed(e);
      }
    });
    systemEmailAlterSettingsButton1.setText("Email Alert Settings");
    systemEmailAlterSettingsButton1.setMaximumSize(new Dimension(217, 27));
    systemEmailAlterSettingsButton1.setMinimumSize(new Dimension(217, 27));
    systemEmailAlterSettingsButton1.setPreferredSize(new Dimension(217, 27));
    systemEmailAlterSettingsButton1.setActionCommand("systemEmailAlertSettingsButton");
    systemEmailAlterSettingsButton1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        systemEmailAlterSettingsButton1_actionPerformed(e);
      }
    });
    systemLockSystemButton1.setText("Lock Screen");
    systemLockSystemButton1.setActionCommand("systemLockSystemButton");
    systemLockSystemButton1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        systemLockSystemButton1_actionPerformed(e);
      }
    });
    systemViewLogsButton1.setText("View SnappCluster Logs");
    systemViewLogsButton1.setActionCommand("systemViewLogsButton");
    systemViewLogsButton1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        systemViewLogsButton1_actionPerformed(e);
      }
    });
    systemShutdownSnappClusterButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        systemShutdownSnappClusterButton_actionPerformed(e);
      }
    });
    systemShutdownSnappClusterButton.setText("Shutdown");
    systemTestFailoverButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        systemTestFailoverButton_actionPerformed(e);
      }
    });
    systemTestFailoverButton.setText("Restart / Failover Test");
    systemBackupButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        systemBackupButton_actionPerformed(e);
      }
    });
    systemBackupButton.setText("Backup Export Options");
    mountFloppyToggle.setActionCommand("mountFloppyToggle");
    mountFloppyToggle.setText("Mount Floppy");
    mountFloppyToggle.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        mountFloppyToggle_actionPerformed(e);
      }
    });
    mountCdToggle.setActionCommand("mountCdToggle");
    mountCdToggle.setText("Mount CD ROM");
    mountCdToggle.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        mountCdToggle_actionPerformed(e);
      }
    });
    /** @todo Adding Parent Components to AppFrame */
    contentPane.add(new JLabel(), new XYConstraints(11, 356, 152, 135));
    contentPane.add(new JLabel(), new XYConstraints(7, 332, 137, 27));
    contentPane.add(jTree1,       new XYConstraints(9, 31, 153, 304));
    contentPane.add(servicesLabel,new XYConstraints(8, 10, 88, 19));
    contentPane.add(jTabbedPane1, new XYConstraints(169, 5, 531, 488));

/** @todo Setting up Lights */
    LightPtr.setDebugGraphicsOptions(DebugGraphics.NONE_OPTION);
    LightPtr.setPreferredSize(new Dimension(300, 300));


/** @todo Setting up Tabbed Panel in Main GUI */

    /** @todo Adding Status JComponents */
    jPanel_Status.add(sStatsPtr, new XYConstraints(11, 342, -1, 112));
    jPanel_Status.add(LightPtr,    new XYConstraints(-14, 8, 500, 500));

    /** @todo Adding Network JComponents */
    jPanel_Network.add(networkApplyButton,          new XYConstraints(97, 405, 125, 41));
    jPanel_Network.add(networkCancelButton,         new XYConstraints(272, 405, 125, 41));
    jPanel_Network.add(altDNSLabel, new XYConstraints(5, 304, 92, 18));
    jPanel_Network.add(period5_2, new XYConstraints(166, 308, 10, 18));
    jPanel_Network.add(period5_3, new XYConstraints(203, 308, 10, 18));
    jPanel_Network.add(primaryDNSLabel, new XYConstraints(5, 273, 92, 18));
    jPanel_Network.add(period4_1, new XYConstraints(129, 276, 10, 18));
    jPanel_Network.add(period4_2, new XYConstraints(166, 276, 10, 18));
    jPanel_Network.add(period4_3, new XYConstraints(203, 276, 10, 18));
    jPanel_Network.add(gatewayIPAddrLabel, new XYConstraints(5, 237, 92, 18));
    jPanel_Network.add(period3_1, new XYConstraints(129, 240, 10, 18));
    jPanel_Network.add(period3_2, new XYConstraints(166, 240, 10, 18));
    jPanel_Network.add(period3_3, new XYConstraints(204, 240, 10, 18));
    jPanel_Network.add(subnetIPAddrLabel, new XYConstraints(5, 200, 92, 18));
    jPanel_Network.add(period2_1, new XYConstraints(129, 203, 10, 18));
    jPanel_Network.add(period2_2, new XYConstraints(166, 203, 10, 18));
    jPanel_Network.add(period2_3, new XYConstraints(204, 203, 10, 18));
    jPanel_Network.add(machIPAddrLabel, new XYConstraints(5, 164, 92, 18));
    jPanel_Network.add(period1_1, new XYConstraints(129, 167, 10, 18));
    jPanel_Network.add(thisMachineNameValue, new XYConstraints(103, 131, 182, 18));
    jPanel_Network.add(machineNameLabel, new XYConstraints(5, 131, 92, 18));
    jPanel_Network.add(period3_4, new XYConstraints(361, 240, 10, 18));
    jPanel_Network.add(period4_4, new XYConstraints(361, 276, 10, 18));
    jPanel_Network.add(period5_4, new XYConstraints(361, 308, 10, 18));
    jPanel_Network.add(period2_4, new XYConstraints(361, 203, 10, 18));
    jPanel_Network.add(period5_5, new XYConstraints(398, 308, 10, 18));
    jPanel_Network.add(period4_5, new XYConstraints(398, 276, 10, 18));
    jPanel_Network.add(period3_5, new XYConstraints(398, 240, 10, 18));
    jPanel_Network.add(period2_5, new XYConstraints(398, 203, 10, 18));
    jPanel_Network.add(period5_6, new XYConstraints(436, 308, 10, 18));
    jPanel_Network.add(period4_6, new XYConstraints(436, 276, 10, 18));
    jPanel_Network.add(period3_6, new XYConstraints(436, 240, 10, 18));
    jPanel_Network.add(period2_6, new XYConstraints(436, 203, 10, 18));
    jPanel_Network.add(otherMachineNameValue, new XYConstraints(335, 131, 182, 18));
    jPanel_Network.add(period5_1, new XYConstraints(129, 308, 10, 18));
    jPanel_Network.add(period1_2, new XYConstraints(166, 167, 10, 18));
    jPanel_Network.add(period1_3, new XYConstraints(204, 167, 10, 18));
    jPanel_Network.add(period1_4, new XYConstraints(361, 167, 10, 18));
    jPanel_Network.add(period1_5, new XYConstraints(398, 167, 10, 18));
    jPanel_Network.add(period1_6, new XYConstraints(436, 167, 10, 18));
    jPanel_Network.add(clusterNameValue, new XYConstraints(257, 22, 182, 18));
    jPanel_Network.add(clusterNameLabel, new XYConstraints(149, 22, 92, 18));
    jPanel_Network.add(clusterIPAddrLabel, new XYConstraints(149, 56, 92, 18));
    jPanel_Network.add(periodC_1, new XYConstraints(283, 59, 10, 18));
    jPanel_Network.add(periodC_2, new XYConstraints(320, 59, 10, 18));
    jPanel_Network.add(periodC_3, new XYConstraints(357, 59, 10, 18));
    jPanel_Network.add(thisLabel,   new XYConstraints(105, 96, 176, 27));
    jPanel_Network.add(otherLabel,   new XYConstraints(340, 96, 174, 27));
    jPanel_Network.add(thisSecondaryDNSValue[1],   new XYConstraints(140, 305, 26, 18));
    jPanel_Network.add(thisSecondaryDNSValue[0],   new XYConstraints(103, 305, 26, 18));
    jPanel_Network.add(thisSecondaryDNSValue[2],   new XYConstraints(177, 305, 26, 18));
    jPanel_Network.add(thisSecondaryDNSValue[3],   new XYConstraints(214, 305, 26, 18));
    jPanel_Network.add(thisPrimaryDNSValue[1],     new XYConstraints(140, 273, 26, 18));
    jPanel_Network.add(thisPrimaryDNSValue[0],    new XYConstraints(103, 273, 26, 18));
    jPanel_Network.add(thisPrimaryDNSValue[2],     new XYConstraints(177, 273, 26, 18));
    jPanel_Network.add(thisPrimaryDNSValue[3],     new XYConstraints(214, 273, 26, 18));
    jPanel_Network.add(thisGatewayIPAddrValue[3],  new XYConstraints(214, 237, 26, 18));
    jPanel_Network.add(thisGatewayIPAddrValue[0],  new XYConstraints(103, 237, 26, 18));
    jPanel_Network.add(thisGatewayIPAddrValue[1],  new XYConstraints(140, 237, 26, 18));
    jPanel_Network.add(thisSubnetIPAddrValue[0],   new XYConstraints(103, 200, 26, 18));
    jPanel_Network.add(thisSubnetIPAddrValue[1],   new XYConstraints(140, 200, 26, 18));
    jPanel_Network.add(thisSubnetIPAddrValue[2],   new XYConstraints(177, 200, 26, 18));
    jPanel_Network.add(thisSubnetIPAddrValue[3],   new XYConstraints(214, 200, 26, 18));
    jPanel_Network.add(thisIPAddrValue[0],         new XYConstraints(103, 164, 26, 18));
    jPanel_Network.add(thisIPAddrValue[1],         new XYConstraints(140, 164, 26, 18));
    jPanel_Network.add(thisIPAddrValue[2],         new XYConstraints(177, 164, 26, 18));
    jPanel_Network.add(thisIPAddrValue[3],         new XYConstraints(214, 164, 26, 18));
    jPanel_Network.add(otherGatewayIPAddrValue[1],   new XYConstraints(372, 237, 26, 18));
    jPanel_Network.add(otherSubnetIPAddrValue[1],    new XYConstraints(372, 200, 26, 18));
    jPanel_Network.add(otherSecondaryDNSValue[0],    new XYConstraints(335, 305, 26, 18));
    jPanel_Network.add(otherIPAddrValue[0],          new XYConstraints(335, 164, 26, 18));
    jPanel_Network.add(otherGatewayIPAddrValue[0],   new XYConstraints(335, 237, 26, 18));
    jPanel_Network.add(otherSubnetIPAddrValue[0],    new XYConstraints(335, 200, 26, 18));
    jPanel_Network.add(otherSecondaryDNSValue[1],    new XYConstraints(372, 305, 26, 18));
    jPanel_Network.add(otherIPAddrValue[2],          new XYConstraints(410, 164, 26, 18));
    jPanel_Network.add(otherSubnetIPAddrValue[2],    new XYConstraints(410, 200, 26, 18));
    jPanel_Network.add(otherGatewayIPAddrValue[2],   new XYConstraints(410, 237, 26, 18));
    jPanel_Network.add(otherPrimaryDNSValue[2],      new XYConstraints(410, 273, 26, 18));
    jPanel_Network.add(otherSecondaryDNSValue[2],    new XYConstraints(410, 305, 26, 18));
    jPanel_Network.add(otherIPAddrValue[3],          new XYConstraints(447, 164, 26, 18));
    jPanel_Network.add(otherSubnetIPAddrValue[3],    new XYConstraints(447, 200, 26, 18));
    jPanel_Network.add(otherGatewayIPAddrValue[3],   new XYConstraints(447, 237, 26, 18));
    jPanel_Network.add(otherPrimaryDNSValue[3],      new XYConstraints(447, 273, 26, 18));
    jPanel_Network.add(otherSecondaryDNSValue[3],    new XYConstraints(447, 305, 26, 18));
    jPanel_Network.add(thisGatewayIPAddrValue[2],  new XYConstraints(177, 237, 26, 18));
    jPanel_Network.add(otherPrimaryDNSValue[0],      new XYConstraints(335, 273, 26, 18));
    jPanel_Network.add(otherPrimaryDNSValue[1],      new XYConstraints(372, 273, 26, 18));
    jPanel_Network.add(otherIPAddrValue[1],          new XYConstraints(372, 164, 26, 18));
    jPanel_Network.add(clusterIPAddrValue[0],       new XYConstraints(257, 56, 26, 18));
    jPanel_Network.add(clusterIPAddrValue[1],       new XYConstraints(294, 56, 26, 18));
    jPanel_Network.add(clusterIPAddrValue[2],       new XYConstraints(331, 56, 26, 18));
    jPanel_Network.add(clusterIPAddrValue[3],       new XYConstraints(368, 56, 26, 18));
    this.loadNetworkTab();

    /** @todo Adding System JComponents */
    int left=93, right=321, top = -11, size=40, spacing=15;
    jPanel_System.add(systemChgPwdButton,                  new XYConstraints(left, top+=size+spacing, right, size));
    jPanel_System.add(systemViewLogsButton1,               new XYConstraints(left, top+=size+spacing, right, size));
    //jPanel_System.add(systemBackupButton,                  new XYConstraints(left, top+=size+spacing, right, size));
    jPanel_System.add(systemEmailAlterSettingsButton1,     new XYConstraints(left, top+=size+spacing, right, size));
    jPanel_System.add(systemLockSystemButton1,             new XYConstraints(left, top+=size+spacing, right, size));
    jPanel_System.add(systemTestFailoverButton,            new XYConstraints(left, top+=size+spacing, right, size));
    jPanel_System.add(systemShutdownSnappClusterButton,    new XYConstraints(left, top+=size+spacing, right, size));
    jPanel_System.add(mountCdToggle,     new XYConstraints(364, 431, 160, -1));
    jPanel_System.add(mountFloppyToggle,        new XYConstraints(201, 431, 157, -1));

    /** @todo Adding Application JComponents */
    jPanel_Application.add(jList_Application,           new XYConstraints(25, 27, 474, 317));
    jPanel_Application.add(applicationsInstallButton,   new XYConstraints(25, 347, 155, 41));
    jPanel_Application.add(applicationsConfigureButton, new XYConstraints(183, 347, 155, 41));
    jPanel_Application.add(applicationsUninstallButton, new XYConstraints(341, 347, 159, 40));

    /** @todo Adding Panels */
    jTabbedPane1.add(jPanel_Status,       "Status");
    jTabbedPane1.add(jPanel_Network,      "Network");
    jTabbedPane1.add(jPanel_Application,  "Applications");
    jTabbedPane1.add(jPanel_System,       "System");

    /** @todo Starting Unit as Beta; all settings uneditable on Beta */
    this.setClusterSettings(false);
    this.setTheseSettings(false);
    this.setOtherSettings(false);

    /** @todo Set Input Mask on all IP input fields */
    int i;
    for(i=0; i<4; i++) this.clusterIPAddrValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.thisIPAddrValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.thisSubnetIPAddrValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.thisGatewayIPAddrValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.thisPrimaryDNSValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.thisSecondaryDNSValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.otherIPAddrValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.otherSubnetIPAddrValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.otherGatewayIPAddrValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.otherPrimaryDNSValue[i].setInputVerifier(ipv);
    for(i=0; i<4; i++) this.otherSecondaryDNSValue[i].setInputVerifier(ipv);


    jTabbedPane1.setSelectedIndex(0);
    modules = SnapptixAPI.getModules();
    this.populateScreen();
    this.NS_CreateButtonGroup();
    this.LOADINIT();
    jTabbedPane1.setSelectedComponent(jPanel_Status);

    /** @todo Set Heartbeat Statistics */
    if ( SnapptixAPI.getMaxNodes() < 2) {
          Light.Set_1_Green();
          this.setClusterSettings(true); // other never turned on
          this.setTheseSettings(true); // other never turned on
    } else {
      //Set Heartbeat statistics
        // determine role
        if ( SnapptixAPI.isAlpha() ) {
          sStats.setRole(AlphaUnit);
        }
        else {
          sStats.setRole(BetaUnit);
        }
        //Set Heartbeat statistics
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
          him = 'R'; // it's down!
        }
        Light.set(SnapptixAPI.networkConfiguration.getThisUnitNum(),  me );
        Light.set(SnapptixAPI.networkConfiguration.getOtherUnitNum(), him);
      } // end maxnodes = 2;
  }
  /**Overridden so we can exit when window is closed*/
  protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    if (e.getID() == WindowEvent.WINDOW_CLOSING) {
      System.exit(0);
    }
  }
  public void resetApplist() {
    this.applist.clear();
  }

  public void populateScreen(){
    try {
      for(int i=0; i < modules.size(); i++) {
        PT.add_Running( ((AppModule)modules.get(i)).getFriendlyName() );
        applist.add(    ((AppModule)modules.get(i)).getFriendlyName() );
      }
      jList_Application.setListData(applist);
    } catch(Exception e) {
      e.printStackTrace();
    }
  }

  void NS_CreateButtonGroup(){
  }

  void LOADINIT()
  {
    boolean RUNNING = true;
    try{
      // This is how often the Screen is updated!
      TM.schedule( SysMon,0, GLOBAL.guiREFRESHTIME);
    }
    catch(Exception e)
    {
    System.out.println("ERROR STARTING TIMER TASK - CRITICIAL FAILURE!");
      e.printStackTrace();
    }
  }
/** @todo Action Performers */
  void networkApplyButton_actionPerformed(ActionEvent e) {
    this.saveNetworkTab();
    SnapptixAPI.networkConfiguration.save();
    SnapptixAPI.unixSystem.configureNetworkSettings();
  }
  void networkCancelButton_actionPerformed(ActionEvent e) {
    this.loadNetworkTab();
  }
  void applicationsUninstallButton_actionPerformed(ActionEvent e) {
    if(this.jList_Application.getSelectedIndex() < 0)
      return;
    if (SnapptixAPI.isBeta()) {
      ErrorBox.state("Changes can only be made to the alpha server.",this);
      return;
    }
    for (int i=0; i<modules.size(); i++) {
      if(this.jList_Application.getSelectedValue().equals(((AppModule)modules.get(i)).getFriendlyName())){
       if ( ((AppModule)modules.get(i)).uninstall() ) {
         ErrorBox.state("Could not uninstall.",this);
         return;
       }
       //otherwise...
       PT.remove_Running(this.jList_Application.getSelectedValue());
       jList_Application.remove(jList_Application.getSelectedIndex());
       jList_Application.repaint();
       for(int j=0; j < applist.size(); j++) {
         if (((AppModule)modules.get(i)).getFriendlyName().equals((String)applist.get(j))) {
           applist.remove(j);
           break;
         }
       }
      }
    }
  }
  void applicationsConfigureButton_actionPerformed(ActionEvent e) {
    if(this.jList_Application.getSelectedIndex() <= 0)
      return;
    if (SnapptixAPI.isBeta()) {
      ErrorBox.state("Changes can only be made to the alpha server.",this);
      return;
    }
    for (int i=0; i<modules.size(); i++) {
      if(this.jList_Application.getSelectedValue().equals(
          ((AppModule)modules.get(i)).getFriendlyName())){
               ((AppModule)modules.get(i)).showWindow();
      }
    }
  }
  void applicationsInstallButton_actionPerformed(ActionEvent e) {
    if (SnapptixAPI.isBeta() ){
      ErrorBox.state("Changes can only be made to the alpha server.",this);
      return;
    }
    JFileChooser fc = new JFileChooser( AppModule.SPKpath );
    fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
    FilterFiles filter = new FilterFiles();
    filter.addExtension("spk");
    filter.setDescription("Snapptix Service Packages");
    fc.setFileFilter(filter);
    fc.setControlButtonsAreShown(false);
    fc.setDialogTitle("Choose an application module:");
    int iResult = fc.showDialog(this, "Install");
    if (iResult == fc.CANCEL_OPTION)
      return;
    //otherwise...
    String pkg = fc.getSelectedFile().toString();
    if(ConfirmBox.ask("Would you like to install package"+pkg+"?", this)) {
      try {
        int i;
        boolean alreadyListed;
        StringTokenizer splitter = new StringTokenizer(pkg, ".");
        AppModule newApp = AppModule.load(splitter.nextToken());
        if(newApp.install()) throw new Exception();
        alreadyListed = false;
        for(i=0; i< modules.size(); i++) {
          if( ((AppModule)modules.get(i)).getPackageName().equals(pkg) ) {
            alreadyListed = true;
            break;
          }
        }
        if(alreadyListed)
          return; // reinstalled, okay, done!
        // otherwise..
        modules.add( newApp );
        alreadyListed = false;
        for( i=0; i < applist.size(); i++) { // double check!
          if (newApp.getFriendlyName().equals((String)applist.get(i))) {
            alreadyListed=true;
            break;
          }
        }
        if (! alreadyListed ) {
          applist.add((String)newApp.getFriendlyName() );
          this.jList_Application.setListData(applist);
          this.jList_Application.repaint();
          PT.add_Running(newApp.getFriendlyName());
        }
      } catch(Exception ex) {
        ErrorBox.state("Error installing package.", this);
      }
    }
  }

  void systemChgPwdButton_actionPerformed(ActionEvent e) {
    changePassword PasswordBox = new changePassword(this);
    PasswordBox.show();
  }
  void systemEmailAlterSettingsButton1_actionPerformed(ActionEvent e) {
    EmailDialog EmailBox = new EmailDialog();
    EmailBox.show();
  }
  void systemLockSystemButton1_actionPerformed(ActionEvent e) {
    LockDialog LockBox = new LockDialog(this);
    LockBox.show();
  }
  void systemViewLogsButton1_actionPerformed(ActionEvent e) {
    JFileChooser fc = new JFileChooser("/var/log/");
    fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
    FilterFiles filter = new FilterFiles();
    filter.addExtension("txt");
    filter.addExtension("log");
    filter.setDescription("Log & Text Files");
    fc.setFileFilter(filter);
    fc.setControlButtonsAreShown(false);
    fc.setDialogTitle("Choose a log:");
    int iResult = fc.showDialog(this, "View");
    if (iResult != fc.CANCEL_OPTION)  {
      OpenFileBox filebox = new OpenFileBox(this, "Open Log File:", true, fc.getSelectedFile());
      filebox.show();
    }
  }
  void systemShutdownSnappClusterButton_actionPerformed(ActionEvent e) {
    if (ConfirmBox.ask("Are you sure you want to shut down this computer?", this))
      System.exit(0);
  }
  void systemTestFailoverButton_actionPerformed(ActionEvent e) {
    if (ConfirmBox.ask("Are you sure you want to cause a deliberate failover?",this))
      SnapptixAPI.unixSystem.failover();
  }
  void systemBackupButton_actionPerformed(ActionEvent e) {
  }

  void load(JTextField octet[], String address) {
    StringTokenizer splitter = new StringTokenizer(address, ".");
    String s;
    for(int i=0; i<4; i++) {
      s = splitter.nextToken();
      if (s.equalsIgnoreCase("_") ||
          s.equalsIgnoreCase("__") ||
          s.equalsIgnoreCase("___")){
               octet[i].setText("");
      } else
        octet[i].setText(s);
    }
  }
  String save(JTextField octet[]) {
    String s ="";
    for(int i=0; i<4; i++) {
      if (octet[i].getText().equalsIgnoreCase("") ||
          octet[i].getText().equalsIgnoreCase(" ") ||
          octet[i].getText().equalsIgnoreCase("  ") ||
          octet[i].getText().equalsIgnoreCase("   ") ) {
             s += "___";
      } else
             s += octet[i].getText();
      if (i<3) s+= ".";
    }
    return s;
  }
  void loadNetworkTab() {
    // Cluster vals
    this.clusterNameValue.setText( SnapptixAPI.networkConfiguration.ClusterName );
    load(clusterIPAddrValue, SnapptixAPI.networkConfiguration.ClusterIP );
    //these vals
    this.thisMachineNameValue.setText( SnapptixAPI.networkConfiguration.thisHostName );
    load(thisIPAddrValue,          SnapptixAPI.networkConfiguration.thisIP           );
    load(thisSubnetIPAddrValue,    SnapptixAPI.networkConfiguration.thisSubnet       );
    load(thisGatewayIPAddrValue,   SnapptixAPI.networkConfiguration.thisGateway      );
    load(thisPrimaryDNSValue,      SnapptixAPI.networkConfiguration.thisPrimaryDNS   );
    load(thisSecondaryDNSValue,    SnapptixAPI.networkConfiguration.thisSecondaryDNS );
    //other vals
    this.otherMachineNameValue.setText( SnapptixAPI.networkConfiguration.otherHostName );
    load(otherIPAddrValue,          SnapptixAPI.networkConfiguration.otherIP           );
    load(otherSubnetIPAddrValue,    SnapptixAPI.networkConfiguration.otherSubnet       );
    load(otherGatewayIPAddrValue,   SnapptixAPI.networkConfiguration.otherGateway      );
    load(otherPrimaryDNSValue,      SnapptixAPI.networkConfiguration.otherPrimaryDNS   );
    load(otherSecondaryDNSValue,    SnapptixAPI.networkConfiguration.otherSecondaryDNS );
    Light.loadUnitNames();
  }

  void saveNetworkTab() {
    // Check for identical names/IPs!
    if(this.thisMachineNameValue.getText().equalsIgnoreCase(otherMachineNameValue.getText()) ||
       this.thisMachineNameValue.getText().equalsIgnoreCase(clusterNameValue.getText())      ||
       this.otherMachineNameValue.getText().equalsIgnoreCase(clusterNameValue.getText())
       ){
              ErrorBox.state("The IP addresses of two units and the cluster  must all be unique.",this);
              return;
    }
    if(save(clusterIPAddrValue).equalsIgnoreCase(save(thisIPAddrValue))  ||
       save(clusterIPAddrValue).equalsIgnoreCase(save(otherIPAddrValue)) ||
       save(thisIPAddrValue).equalsIgnoreCase(save(otherIPAddrValue))    ){
              ErrorBox.state("The IP addresses of two units and the cluster  must all be unique.",this);
              return;
    }

    // Cluster
    SnapptixAPI.networkConfiguration.ClusterName = this.clusterNameValue.getText();
    SnapptixAPI.networkConfiguration.ClusterIP   = save(clusterIPAddrValue) ;
    // This unit
    SnapptixAPI.networkConfiguration.thisHostName = this.thisMachineNameValue.getText();
    SnapptixAPI.networkConfiguration.thisIP           = save(thisIPAddrValue);
    SnapptixAPI.networkConfiguration.thisSubnet       = save(thisSubnetIPAddrValue);
    SnapptixAPI.networkConfiguration.thisGateway      = save(thisGatewayIPAddrValue);
    SnapptixAPI.networkConfiguration.thisPrimaryDNS   = save(thisPrimaryDNSValue);
    SnapptixAPI.networkConfiguration.thisSecondaryDNS = save(thisSecondaryDNSValue);
    // Other unit
    SnapptixAPI.networkConfiguration.otherHostName = this.otherMachineNameValue.getText();
    SnapptixAPI.networkConfiguration.otherIP           = save(otherIPAddrValue);
    SnapptixAPI.networkConfiguration.otherSubnet       = save(otherSubnetIPAddrValue);
    SnapptixAPI.networkConfiguration.otherGateway      = save(thisGatewayIPAddrValue);
    SnapptixAPI.networkConfiguration.otherPrimaryDNS   = save(otherPrimaryDNSValue);
    SnapptixAPI.networkConfiguration.otherSecondaryDNS = save(otherSecondaryDNSValue);
    Light.loadUnitNames();
  }
  public void setClusterSettings(boolean onoff) {
    this.clusterNameValue.setEditable(onoff);
    for(int i=0; i<4; i++) this.clusterIPAddrValue[i].setEditable(onoff);
  }
  public void setTheseSettings(boolean onoff){
    int i;
    this.thisMachineNameValue.setEditable(onoff);
    for(i=0; i<4; i++) this.thisIPAddrValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.thisSubnetIPAddrValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.thisGatewayIPAddrValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.thisPrimaryDNSValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.thisSecondaryDNSValue[i].setEditable(onoff);
  }
  public void setOtherSettings(boolean onoff){
    int i;
    this.otherMachineNameValue.setEditable(onoff);
    for(i=0; i<4; i++) this.otherIPAddrValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.otherSubnetIPAddrValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.otherGatewayIPAddrValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.otherPrimaryDNSValue[i].setEditable(onoff);
    for(i=0; i<4; i++) this.otherSecondaryDNSValue[i].setEditable(onoff);
  }

  void mountFloppyToggle_actionPerformed(ActionEvent e) {
    if (mountFloppyToggle.isSelected()) {
      if(SnapptixAPI.unixSystem.mount("floppy")) {
         mountFloppyToggle.setSelected(false);
         return;
      }
      //otherwise
      mountFloppyToggle.setText("Unmount Floppy");
    } else {
      if(SnapptixAPI.unixSystem.unmount("floppy")) {
        mountFloppyToggle.setSelected(true);
        return;
      }
      //otherwise
      mountFloppyToggle.setText("Mount Floppy");
    }
  }

  void mountCdToggle_actionPerformed(ActionEvent e) {
    if (mountCdToggle.isSelected()) {
      if(SnapptixAPI.unixSystem.mount("cdrom")) {
         mountCdToggle.setSelected(false);
         return;
      }
      //otherwise
      mountCdToggle.setText("Unmount CD ROM");
    } else {
      if(SnapptixAPI.unixSystem.unmount("cdrom")) {
        mountCdToggle.setSelected(true);
        return;
      }
      //otherwise
      mountCdToggle.setText("Mount CD ROM");
    }
  }
}