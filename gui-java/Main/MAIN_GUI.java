package Main;

import Snapptix_frontend.*;
import Snapptix_backend.*;
import javax.swing.UIManager;
import java.awt.*;

public class MAIN_GUI implements Snapptix_frontend.GLOBAL {
  /**Main method*/
  public static void main(String[] args) {
    try {
      UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    }
    catch(Exception e) {
      e.printStackTrace();
    }
    new MAIN_GUI();
  }

  boolean packFrame = false;

  /**Construct the application*/
  public MAIN_GUI() {
    //Validate frames that have preset sizes
    //Pack frames that have useful preferred size info, e.g. from their layout
    if (packFrame) {
      App.pack();
    }
    else {
      App.validate();
    }
    //Center the window
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension frameSize = App.getSize();
    if (frameSize.height > screenSize.height) {
      frameSize.height = screenSize.height;
    }
    if (frameSize.width > screenSize.width) {
      frameSize.width = screenSize.width;
    }
    App.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);
    App.setVisible(true);
  }
}