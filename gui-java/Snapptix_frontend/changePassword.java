package Snapptix_frontend;

import java.awt.*;
import com.borland.jbcl.layout.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;

public class changePassword extends JDialog {
  private JPanel panel1 = new JPanel();
  private XYLayout xYchgPwdLayout = new XYLayout();
  private JButton chgPwdOKButton = new JButton();
  private JButton chgPwdCancelButton = new JButton();
  private JLabel chgPwdCurrentLabel = new JLabel();
  private JLabel chgPwdNew1Label = new JLabel();
  private JLabel chgPwdNew2Label = new JLabel();
  private JPasswordField chgPwdCurrentField = new JPasswordField();
  private JPasswordField chgPwdNew1Field = new JPasswordField();
  private JPasswordField chgPwdNew2Field = new JPasswordField();
  private JLabel chgPwdTitleLabel = new JLabel();
  private TitledBorder titledBorder1;

  public changePassword(AppFrame frame) {
    super(frame, "Change Password", true);
    try {
      jbInit();
      this.setSize(450,300);
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }

  public changePassword() {
    this(null);
  }
  private void jbInit() throws Exception {
    titledBorder1 = new TitledBorder("");
    panel1.setLayout(xYchgPwdLayout);
    chgPwdOKButton.setText("OK");
    chgPwdOKButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        chgPwdOKButton_actionPerformed(e);
      }
    });
    chgPwdCancelButton.setText("Cancel");
    chgPwdCancelButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        chgPwdCancelButton_actionPerformed(e);
      }
    });
    chgPwdCurrentLabel.setText("Enter current password:");
    chgPwdNew1Label.setText("Enter new password:");
    chgPwdNew2Label.setText("Re-enter new password:");
    chgPwdTitleLabel.setBackground(Color.gray);
    chgPwdTitleLabel.setFont(new java.awt.Font("Dialog", 1, 12));
    chgPwdTitleLabel.setBorder(titledBorder1);
    chgPwdTitleLabel.setHorizontalAlignment(SwingConstants.CENTER);
    chgPwdTitleLabel.setText("Change Administrator Password");
    getContentPane().add(panel1);
    panel1.add(chgPwdOKButton,     new XYConstraints(50, 235, 106, 30));
    panel1.add(chgPwdCancelButton, new XYConstraints(228, 235, 106, 30));
    panel1.add(chgPwdNew1Label,      new XYConstraints(5, 130, 163, 18));
    panel1.add(chgPwdCurrentLabel,   new XYConstraints(6, 90, 162, 18));
    panel1.add(chgPwdNew2Label,      new XYConstraints(5, 170, 164, 18));
    panel1.add(chgPwdCurrentField, new XYConstraints(170, 90, 220, 18));
    panel1.add(chgPwdNew1Field,    new XYConstraints(170, 130, 220, 18));
    panel1.add(chgPwdNew2Field,    new XYConstraints(170, 170, 220, 18));
    panel1.add(chgPwdTitleLabel,     new XYConstraints(35, 17, 330, 40));
  }

  void chgPwdOKButton_actionPerformed(ActionEvent e) {

    if( GLOBAL.SnapptixAPI.isPassword( this.chgPwdCurrentField.getText() ) ) {
      chgPwdTitleLabel.setText("Old password incorrect.");
      return;
    }
    if( !this.chgPwdNew1Field.getText().equals(this.chgPwdNew2Field.getText() ) ) {
      chgPwdTitleLabel.setText("New passwords do not match.");
      return;
    }
    if( this.chgPwdNew1Field.getText().length() < 5 ) {
      chgPwdTitleLabel.setText("Password must be at least 5 characters long.");
      return;
    }
    //otherwise....
    GLOBAL.SnapptixAPI.setPassword(chgPwdCurrentField.getText(),
                                   chgPwdNew1Field.getText());
    this.hide();
  }

  void chgPwdCancelButton_actionPerformed(ActionEvent e) {
    this.hide();
  }


}