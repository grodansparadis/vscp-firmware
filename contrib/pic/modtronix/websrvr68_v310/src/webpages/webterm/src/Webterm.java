/*
 * Created on Jan 14, 2005
 */
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.HeadlessException;

import javax.swing.BorderFactory;
import javax.swing.JApplet;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;


/**
 * Main Applet class
 */
public class Webterm extends JApplet {
    JPanel paneTop = new JPanel(); 
    JTextArea txtarea = new JTextArea();
    
    /**
     * @throws java.awt.HeadlessException
     */
    public Webterm() throws HeadlessException {
        super();
        
        //getRootPane().putClientProperty("defeatSystemEventQueueCheck",
        //        Boolean.TRUE);
    }
    
    public void init() {
        /////////////////////////////////////////////////
        //Configure label
        JLabel label = new JLabel("Web Based Terminal");
        label.setHorizontalAlignment(JLabel.CENTER);

        /////////////////////////////////////////////////
        //Configure panetop
        //txtarea.setBorder(BorderFactory.createMatteBorder(1,1,2,2,Color.black));
        paneTop.setLayout(new BorderLayout());
        paneTop.add(label, BorderLayout.NORTH);
        paneTop.add(txtarea, BorderLayout.CENTER);
        getContentPane().add(paneTop, BorderLayout.CENTER);
        
        txtarea.append("hello\r\n");
        txtarea.append("there\r\n");
        txtarea.append("CodeBase: " + this.getCodeBase().toString() + "\r\n");
        txtarea.append("DocumentBase: " + this.getDocumentBase().toString() + "\r\n");
    }
}
