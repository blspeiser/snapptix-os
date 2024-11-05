package Snapptix_frontend;

import java.awt.*;
import com.borland.jbcl.layout.*;
import javax.swing.*;

public class BEAN_ServerStatistics extends JPanel implements GLOBAL {

  XYLayout serverStatisticsLayout = new XYLayout();

  private JLabel currentRoleLabel = new JLabel();
  private JLabel currentRoleValue = new JLabel();
  private JLabel CPUUsageLabel = new JLabel();
  private JLabel CPUUsageValue = new JLabel();
  private JLabel memoryUsageLabel = new JLabel();
  private JLabel memoryUsageValue = new JLabel();
  private JLabel numbActiveProcessesLabel = new JLabel();
  private JLabel numbActiveProcessesValue = new JLabel();
  private JLabel networkThrouhputlabel = new JLabel();
  private JLabel networkThroughputValue = new JLabel();

  public BEAN_ServerStatistics() {
    try {
      jbInit();
      this.repaint();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }
  private void jbInit() throws Exception {

    this.setLayout(serverStatisticsLayout);

    serverStatisticsLayout.setWidth(494);
    serverStatisticsLayout.setHeight(120);

    currentRoleLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    currentRoleLabel.setDoubleBuffered(true);
    currentRoleLabel.setText("This computer acting as: ");
    this.add(currentRoleLabel,     new XYConstraints(5, 10, 205, 18));

    CPUUsageLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    CPUUsageLabel.setText("CPU usage:");
    this.add(CPUUsageLabel,   new XYConstraints(5, 30, 215, 18));

    memoryUsageLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    memoryUsageLabel.setText("Memory usage:");
    this.add(memoryUsageLabel,   new XYConstraints(5, 50, 215, 18));

    numbActiveProcessesLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    numbActiveProcessesLabel.setText("Number of active processes:");
    this.add(numbActiveProcessesLabel,    new XYConstraints(5, 70, 215, 18));

    networkThrouhputlabel.setFont(new java.awt.Font("Dialog", 1, 12));
    networkThrouhputlabel.setText("Network throughput on each NIC:");
    this.add(networkThrouhputlabel,  new XYConstraints(5, 90, 215, 18));

    if( SnapptixAPI.isAlpha() )
      currentRoleValue.setText(AlphaUnit);
    else
      currentRoleValue.setText(BetaUnit);

    CPUUsageValue.setText("...");
    memoryUsageValue.setText("... MB");
    numbActiveProcessesValue.setText("...");
    networkThroughputValue.setText("... Kb");

    this.add(currentRoleValue,          new XYConstraints(230, 10, 246, 18));
    this.add(CPUUsageValue,            new XYConstraints(230, 30, 110, 18));
    this.add(memoryUsageValue,         new XYConstraints(230, 50, 110, 18));
    this.add(numbActiveProcessesValue, new XYConstraints(230, 70, 110, 18));
    this.add(networkThroughputValue,    new XYConstraints(230, 90, 241, 18));
  }

 public void setRole(String role) {
    currentRoleValue.setText(role);
 }
 public void setMem(String mem) {
    memoryUsageValue.setText(mem);
 }
 public void setCPU(String cpu) {
     CPUUsageValue.setText(cpu);
 }
 public void setProcs(String procs) {
     numbActiveProcessesValue.setText(procs);
 }
 public void setThroughput(String net) {
     networkThroughputValue.setText(net);
 }
}



