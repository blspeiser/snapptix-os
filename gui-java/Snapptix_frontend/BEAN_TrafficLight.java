package Snapptix_frontend;

import java.awt.*;
import com.borland.jbcl.layout.*;
import javax.swing.*;

public class BEAN_TrafficLight extends JPanel implements GLOBAL {
  XYLayout  xYLayout1 = new XYLayout();
  XYLayout  xYLayout2 = new XYLayout();
  XYLayout  xYLayout3 = new XYLayout();
  XYLayout  xYLayout4 = new XYLayout();
  tl1       Light1    = new tl1();
  tl1       Light2    = new tl1();
  BEAN_Line line      = new BEAN_Line();
  JLabel leftNodeLabel = new JLabel();
  JLabel rightNodeLabel= new JLabel();
  int numberOfServers;

  public BEAN_TrafficLight(int nodes) {
    this.numberOfServers = nodes;
    try {
      jbInit();
      this.Set_1_Green();
      this.Set_2_Green();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    if (numberOfServers == 2) {
      this.setMinimumSize(new Dimension(300, 300));
      this.setPreferredSize(new Dimension(522, 250));
      this.setLayout(xYLayout4);
      xYLayout4.setWidth(549);
      xYLayout4.setHeight(292);
      line.setForeground(Color.green);
      leftNodeLabel.setHorizontalAlignment(SwingConstants.CENTER);
      rightNodeLabel.setHorizontalAlignment(SwingConstants.CENTER);
      this.add(Light1, new XYConstraints(-36, 0, 237, 288));
      this.add(Light2, new XYConstraints(296, 0, 279, 327));
      this.add(line, new XYConstraints(165, 132, 224, 157));
      this.loadUnitNames();
      this.add(leftNodeLabel, new XYConstraints(55, 15, 110, 19));
      this.add(rightNodeLabel, new XYConstraints(386, 20, 109, -1));
    } else {
      this.setMinimumSize(new Dimension(300, 300));
      this.setPreferredSize(new Dimension(522, 250));
      this.setLayout(xYLayout4);
      xYLayout4.setWidth(549);
      xYLayout4.setHeight(292);
      leftNodeLabel.setHorizontalAlignment(SwingConstants.CENTER);
      leftNodeLabel.setText("Single Server");
      this.add(Light1, new XYConstraints(100, 0, 237, 288));
      this.add(leftNodeLabel, new XYConstraints(191, 15, 110, 19));
   }
  }
  public void addHeartbeat(){
    line.setVisible(true);
    line.repaint();
  }
  public void killHeartbeat(){
    line.setVisible(false) ;
    line.repaint();
  }
  public void Set_1_Red(){
    Light1.state = 0;
    Light1.repaint();
  }
  public void Set_1_Green(){
    Light1.state = 2;
    Light1.repaint();
  }
  public void Set_1_Yellow(){
    Light1.state = 1;
    Light1.repaint();
  }
  public void Set_2_Red(){
    Light2.state = 0;
    Light2.repaint();
  }
  public void Set_2_Green(){
    Light2.state = 2;
    Light2.repaint();
  }
  public void Set_2_Yellow(){
    Light2.state = 1;
    Light2.repaint();
  }
  public void loadUnitNames() {
    if ( SnapptixAPI.networkConfiguration.getThisUnitNum() == 1) {
      leftNodeLabel.setText( "Unit 1: " + SnapptixAPI.networkConfiguration.thisHostName );
      rightNodeLabel.setText("Unit 2: " + SnapptixAPI.networkConfiguration.otherHostName);
    } else { // UnitNum := 2
      leftNodeLabel.setText( "Unit 1: " + SnapptixAPI.networkConfiguration.otherHostName);
      rightNodeLabel.setText("Unit 2: " + SnapptixAPI.networkConfiguration.thisHostName );
    }
    this.repaint();
  }
  public void set(int light, char color) {
    switch (color) {
      case 'G':
        if(light == 1) this.Set_1_Green();
        if(light == 2) this.Set_2_Green();
        break;
      case 'Y':
        if(light == 1) this.Set_1_Yellow();
        if(light == 2) this.Set_2_Yellow();
        break;
      case 'R':
        if(light == 1) this.Set_1_Red();
        if(light == 2) this.Set_2_Red();
        break;
      default:
        break;
    }
  }
}

