package Snapptix_frontend;

import javax.swing.*;
import java.awt.*;
import com.borland.jbcl.layout.*;
import java.awt.event.*;

public class LockDialog  {
  private JDialog JD;
  private JPanel jPanel1 = new JPanel();
  private BorderLayout borderLayout1 = new BorderLayout();
  private JPanel jPanel2 = new JPanel();
  private JButton jButton_OK = new JButton();
  private JLabel jLabel1 = new JLabel();
  private JPasswordField jPasswordField1 = new JPasswordField();
  private JLabel LBL_passResult = new JLabel();

  public LockDialog(AppFrame owner) throws HeadlessException {
    JD = new JDialog(owner,true);
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    JD.setSize(screenSize);
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  public LockDialog() throws HeadlessException {
    this(GLOBAL.App);
  }

  private void jbInit() throws Exception {
    jPanel1.setLayout(borderLayout1);
    jButton_OK.setText("Log In");
    jButton_OK.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButton_OK_actionPerformed(e);
      }
    });
    jLabel1.setText("Enter Password");
    jPasswordField1.setColumns(15);
    jPasswordField1.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusGained(FocusEvent e) {
        jPasswordField1_focusGained(e);
      }
    });

    LBL_passResult.setHorizontalAlignment(SwingConstants.CENTER);
    JD.getContentPane().add(jPanel1, BorderLayout.WEST);
    jPanel2.add(jLabel1, null);
    jPanel2.add(jPasswordField1, null);
    jPanel2.add(jButton_OK, null);
    JD.getContentPane().add(LBL_passResult, BorderLayout.CENTER);
    JD.getContentPane().add(jPanel2, BorderLayout.NORTH);
  }

  void jButton_OK_actionPerformed(ActionEvent e) {
    //Should check password and if ok should then hide itself.
    if(checkPass())
      JD.hide();
  }
  void show(){
     JD.show();
  }

  boolean checkPass(){
    //do pass checking here and  return true if ok and false if not ok
    if(GLOBAL.SnapptixAPI.isPassword(new String (jPasswordField1.getPassword()) ))
      return (true);
    else
      LBL_passResult.setText("Not a Valid Password");
    return (false);
  }

  void jPasswordField1_focusGained(FocusEvent e) {
    LBL_passResult.setText("");
  }
}