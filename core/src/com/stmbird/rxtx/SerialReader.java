package com.stmbird.rxtx;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.io.InputStream;

/**
 * Class that read the incoming data from our STM.
 */

public class SerialReader implements Runnable
{
    InputStream in;

    public SerialReader(InputStream in) {
        this.in = in;
    }

    public void run() {
        byte[] buffer = new byte[1024];
        String READ;
        int len = -1;

        try {
            Robot robot = new Robot();

            while ((len = this.in.read(buffer)) > -1) {
                READ = new String(buffer,0,len);

                if(READ.length()>0)
                {

                    {
                        robot.keyPress(KeyEvent.VK_SPACE);
                        robot.keyRelease(KeyEvent.VK_SPACE);
                    }

                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

}
