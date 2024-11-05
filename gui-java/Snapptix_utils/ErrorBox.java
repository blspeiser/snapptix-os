package Snapptix_utils;

import java.awt.*;
import javax.swing.*;
import com.borland.jbcl.layout.*;
import java.awt.event.*;

public class ErrorBox extends JDialog {
  private JPanel jPanel1 = new JPanel();
  private XYLayout xYLayout1 = new XYLayout();
  private JLabel jLabel1 = new JLabel();
  private JButton jButton1 = new JButton();

  public ErrorBox(Frame frame, String title, boolean modal) {
    super(frame, title, modal);
    try {
      jbInit();
      pack();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }

  public ErrorBox(JFrame parent) {
    this(parent, "Error:", true);
  }
  private void jbInit() throws Exception {
    jPanel1.setLayout(xYLayout1);
    jButton1.setToolTipText("");
    jButton1.setText("OK");
    jButton1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButton1_actionPerformed(e);
      }
    });
    jLabel1.setHorizontalAlignment(SwingConstants.CENTER);
    jLabel1.setHorizontalTextPosition(SwingConstants.CENTER);
    jPanel1.add(jLabel1,      new XYConstraints(13, 87, 363, 49));
    jPanel1.add(jButton1,     new XYConstraints(107, 223, 168, 49));
    this.getContentPane().add(jPanel1, BorderLayout.CENTER);

  }
  public void sError(String STR){
    jLabel1.setText(STR);
  }
  void jButton1_actionPerformed(ActionEvent e) {
    this.hide();
  }
  public static void state(String err) {
    ErrorBox eb = new ErrorBox(null);
    eb.sError(err);
    eb.show();
  }
  public static void state(String err, JFrame parent) {
     ErrorBox eb = new ErrorBox(parent);
     eb.sError(err);
     eb.show();
  }
}


