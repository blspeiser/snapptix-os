package Snapptix_frontend;

import java.awt.*;
import javax.swing.*;
import com.borland.jbcl.layout.*;
import java.awt.event.*;
import Snapptix_utils.*;

public class EmailDialog extends JDialog {
  private JPanel panel1 = new JPanel();
  private XYLayout xYLayout1 = new XYLayout();
  private JTextField textEmailAddress = new JTextField();
  private JLabel jLabel1 = new JLabel();
  private JCheckBox jCheck_SystemCrash = new JCheckBox();
  private JCheckBox jCheck_SendService = new JCheckBox();
  private JButton jButton_OK = new JButton();
  private JButton jButton_Cancel = new JButton();
  private JLabel warning = new JLabel();

  public EmailDialog(Frame frame, String title, boolean modal) {
    super(frame, title, modal);
    try {
      jbInit();
      pack();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }

  public EmailDialog() {
    this(null, "", false);
  }
  private void jbInit() throws Exception {
    panel1.setLayout(xYLayout1);
    textEmailAddress.setText( GLOBAL.SnapptixAPI.mailer.to  );
    textEmailAddress.setToolTipText("");
    textEmailAddress.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        textEmailAddress_actionPerformed(e);
      }
    });
    jLabel1.setText("Email Address:");
    jCheck_SystemCrash.setText("Send on System Crash");
    jCheck_SendService.setText("Send on Service Failure");
    jButton_OK.setText("Apply");
    jButton_OK.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButton_OK_actionPerformed(e);
      }
    });
    jButton_Cancel.setText("Cancel");
    jButton_Cancel.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButton_Cancel_actionPerformed(e);
      }
    });

    panel1.add(jLabel1, new XYConstraints(8, 28, 123, 35));
    //panel1.add(jCheck_SendService, new XYConstraints(115, 95, -1, 24));
    panel1.add(textEmailAddress, new XYConstraints(142, 31, 277, 28));
    panel1.add(warning, new XYConstraints(73, 90, 300, 24));
    panel1.add(jButton_OK, new XYConstraints(79, 123, 111, 49));
    panel1.add(jButton_Cancel,  new XYConstraints(243, 121, 111, 49));
    panel1.add(jCheck_SystemCrash, new XYConstraints(142, 66, 190, 27));
    this.getContentPane().add(panel1, BorderLayout.NORTH);
  }

  void textEmailAddress_actionPerformed(ActionEvent e) {

  }
  void jButton_Cancel_actionPerformed(ActionEvent e) {
    this.hide();
  }

  void jButton_OK_actionPerformed(ActionEvent e) {
    String addr = textEmailAddress.getText();
    if (addr == null || addr.equalsIgnoreCase("") || addr.length() == 0) {
      warning.setText("");
      GLOBAL.SnapptixAPI.mailer.to = "";
      jCheck_SystemCrash.setSelected(false);
      GLOBAL.SnapptixAPI.mailer.mailing = false;
      return;
    }
    int i;
    for(i=0; i < addr.length(); i++) {
      if(addr.charAt(i) == '@')
        break;
    }
    if (i > addr.length() - 5) {// "me@ x.zzz " = 5
      warning.setText(addr + "is not a valid email address.");
      warning.repaint();
      textEmailAddress.selectAll();
      return;
    }
    warning.setText("");
    for(; i < addr.length(); i++) {
      if(addr.charAt(i) == '.')
        break;
    }
    if (i > addr.length() - 3) {// "me@x .zzz " = 3
      warning.setText(addr + "is not a valid email address.");
      warning.repaint();
      textEmailAddress.selectAll();
      return;
    }
    warning.setText("");
    //otherwise...
    GLOBAL.SnapptixAPI.mailer.to      = addr;
    GLOBAL.SnapptixAPI.mailer.mailing = this.jCheck_SystemCrash.isSelected();
    GLOBAL.SnapptixAPI.mailer.save();
    this.hide();
  }
}