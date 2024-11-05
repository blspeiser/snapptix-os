package Snapptix_frontend;

import java.awt.*;
import javax.swing.JPanel;

public class BEAN_Line extends Component {
  BorderLayout borderLayout1 = new BorderLayout();

  public BEAN_Line() {
    try {
      jbInit();
    }
    catch(Exception ex) {
      ex.printStackTrace();
    }
  }
  private void jbInit() throws Exception {
  }

  public void paint(Graphics g) {
    Color c1 = new Color(0,200,0);
    g.setColor(c1);
    g.drawRect(0,0,500,10);
    g.fillRect(0,0,480,8);
  }
}