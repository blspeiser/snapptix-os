package Snapptix_frontend;

import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.text.*;
import java.util.*;

public class ProgramTree extends JPanel {
  BorderLayout borderLayout1 = new BorderLayout();
  DefaultMutableTreeNode top=null;
  JTree jTree1;
  DefaultTreeModel treeModel;

  public ProgramTree() {
    try {
      top= new DefaultMutableTreeNode("Services");
      treeModel=new DefaultTreeModel(top);
      treeModel.setRoot(top);
      jTree1 = new JTree(treeModel);
      jbInit();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
    this.setLayout(borderLayout1);
    this.add(jTree1, BorderLayout.CENTER);
  }

  public void add_Running(Object obj){
    try{
      top.add( new DefaultMutableTreeNode(obj) );
      treeModel.reload();
    } catch(Exception e) {
      e.printStackTrace();
    }
  }

  public void remove_Running(Object obj){
   try{
    String test = ((String)obj) ;
    Enumeration ENUM = top.children();
    int i = 0;
    while(ENUM.hasMoreElements()){
      String temp = ENUM.nextElement().toString();
      if(temp.matches(test)) {
        top.remove(i);
      }
      ++i;
    }
    treeModel.reload();
   } catch(Exception e) {
      e.printStackTrace();
    }
  }

  public void resetList(){
    try{
      Enumeration ENUM = top.children();
      int i = 0;
      while(ENUM.hasMoreElements()){
          top.remove(i++);
      }
      treeModel.reload();
    } catch(Exception e) {
      e.printStackTrace();
    }
  }
}