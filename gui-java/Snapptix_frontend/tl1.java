package Snapptix_frontend;

import java.awt.*;
import javax.swing.JPanel;

public class tl1 extends Component {
    BorderLayout borderLayout1 = new BorderLayout();
    static final int RED = 0;
    static final int YELLOW = 1;
    static final int GREEN = 2;
    int state = 0;

    public tl1() {
        try {
            jbInit();
        }
        catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    public int getState() {
        return state;
    }
    public void setState(int state) {
        this.state = state;
        repaint();
    }
    public void cycle() {
        state++;
        if (state > GREEN) state = RED;
        repaint();
    }
    public void paint(Graphics g) {
        g.drawRect(91,42,109,223);
        g.setColor(Color.darkGray);
        g.fillRect(95,47,102,215);
        if (state == GREEN) {
            g.setColor(Color.green);
            g.fillOval(120,55,55,55);
        }
        if (state == YELLOW) {
            g.setColor(Color.yellow);
            g.fillOval(120,128,55,55);
        }
        if (state == RED) {
            g.setColor(Color.red);
            g.fillOval(120,200,55,55);
        }
    }
    public Dimension getPreferredSize() {
        return new Dimension(100,100);
    }
    private void jbInit() throws Exception {
    }
}