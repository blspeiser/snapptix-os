package Snapptix_frontend;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import snapptix_CORE.*;
import com.borland.jbcl.layout.*;
import java.util.*;
import java.io.*;

/**
 * Title:
 * Description:
 * Copyright:    Copyright (c) 2003
 * Company:
 * @author
 * @version 1.0
 */

public class BEAN_Logging extends JPanel implements GLOBAL{
  private String sample = "Sample";
  JScrollPane jScrollPane1 = new JScrollPane();
  JTextArea jTextLog = new JTextArea();
  OutputStream FO;

  public int A = 0;
  JLabel jLabel1 = new JLabel();
  XYLayout xYLayout1 = new XYLayout();


  public BEAN_Logging() {
    try {
      jbInit();
      this.repaint();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setLayout(xYLayout1);
    jTextLog.addFocusListener(new java.awt.event.FocusAdapter() {
      public void focusGained(FocusEvent e) {
        jTextLog_focusGained(e);
      }
    });

    jLabel1.setMaximumSize(new Dimension(101, 17));
    jLabel1.setMinimumSize(new Dimension(51, 17));
    jLabel1.setPreferredSize(new Dimension(51, 17));
    jLabel1.setHorizontalAlignment(SwingConstants.LEFT);
    jLabel1.setText("Log:");
    xYLayout1.setWidth(494);
    xYLayout1.setHeight(120);
    this.add(jScrollPane1, new XYConstraints(0, 17, 487, 97));
    this.add(jLabel1, new XYConstraints(0, 0, 487, -1));
    jScrollPane1.getViewport().add(jTextLog, null);
  }

  public void setLogText(String t) {
    Date dt = new Date();
    String str = dt.toString()  +' '+t+'\n'+'\r';
    jTextLog.append(str);
    this.write(str);

  }

  public void PrintError(String Err){
    Date dt = new Date();
    String str = dt.toString()+"**** "+Err + '\n' +'\r';
    this.jTextLog.append(str);
    this.write(str);

  }

  void jTextLog_focusGained(FocusEvent e) {
    PT.requestFocus();

  }

  void write(String msg){
   Date dt = new Date();
   Calendar CAL = Calendar.getInstance();
   CAL.setTime(dt);
   int Day =CAL.get(CAL.DAY_OF_MONTH); //dt.getDate();
   int Month = CAL.get(CAL.MONTH);//dt.getMonth();
   int Year = CAL.get(CAL.YEAR);//dt.getYear();
   System.out.println(Year);
   String FName =LOGGERPATH + String.valueOf(Month)+"_"+String.valueOf(Day)+"_"+String.valueOf(Year)+".LOG";
   File log = new File(FName);


    try{
    log.createNewFile();
    //FO = new FileOutputStream(log.getPath());
    //FO.write(msg.getBytes());
    //FO.flush();

        BufferedWriter out = new BufferedWriter(new FileWriter(log.getPath(), true));
        out.write(msg,0,msg.length());
        out.flush();

         out.close();

    }
    catch(IOException e){
      e.printStackTrace();
    }
    }

    protected void finalize(){
      try{


      }
      catch(Exception e)
      {
        e.printStackTrace();
      }
    }

}