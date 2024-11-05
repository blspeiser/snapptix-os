package Snapptix_utils;

import java.awt.*;
import javax.swing.*;
import com.borland.jbcl.layout.*;
import java.io.*;
import java.awt.event.*;

public class OpenFileBox extends JDialog {
  private JPanel panel1 = new JPanel();
  private BorderLayout borderLayout1 = new BorderLayout();
  private JPanel jPanel1 = new JPanel();
  private JButton jButton1 = new JButton();
  private BoxLayout2 boxLayout21 = new BoxLayout2();
  private JScrollPane jScrollPane1 = new JScrollPane();
  private JTextArea jTextArea1 = new JTextArea();
  private FileInputStream FS;

  public OpenFileBox(JFrame frame, String title, boolean modal, File f) {
    super(frame, title, modal);
    try {
      jbInit();
      pack();
      File file = f;
      int size = (int)file.length();
      int chars_read = 0;
      FileReader in = new FileReader(file);
      char[] data = new char[size];
       while(in.ready()) {
         chars_read += in.read(data, chars_read, size - chars_read);
      }
      in.close();
       jTextArea1.setText(new String(data, 0, chars_read));
       this.setSize(300,300);
    } catch(Exception ex) {
      ex.printStackTrace();
    }
  }

  public OpenFileBox() {
    this(null, "", false, null);
  }
  private void jbInit() throws Exception {
    panel1.setLayout(borderLayout1);
    jPanel1.setLayout(boxLayout21);
    jButton1.setText("Close");
    jButton1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jButton1_actionPerformed(e);
      }
    });
    jTextArea1.setEditable(false);
    jTextArea1.setLineWrap(true);
    getContentPane().add(panel1);
    panel1.add(jPanel1, BorderLayout.CENTER);
    jPanel1.add(jScrollPane1, null);
    jScrollPane1.getViewport().add(jTextArea1, null);
    this.getContentPane().add(jButton1, BorderLayout.SOUTH);
  }

  void jButton1_actionPerformed(ActionEvent e) {
    this.hide();
  }
}