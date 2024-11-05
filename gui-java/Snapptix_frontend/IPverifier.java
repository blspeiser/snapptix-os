package Snapptix_frontend;

import javax.swing.*;
import Snapptix_utils.*;

public class IPverifier extends InputVerifier{
  ErrorBox DGE;
  JTextField JT;

  public IPverifier(JFrame f) {
    DGE = new ErrorBox(f,"Error",true);
  }

  public boolean verify(JComponent JC){
    try{
      JT = (JTextField) JC;
      Integer.parseInt(JT.getText());
      if( JT.getText().length() > 3 ||
          (new Integer(JT.getText())).compareTo(new Integer(255)) > 0 ||
          (new Integer(JT.getText())).compareTo(new Integer(0)) < 0 ) {
         throw new NumberFormatException();
      }
      return true;
    } catch(NumberFormatException e) {
        if(JT.getText().equalsIgnoreCase("") )
          return true;
        //otherwise
        DGE.sError("IP field must be a number ranging from 0 to 255.");
        DGE.show();
        return false;
      }
  }
}