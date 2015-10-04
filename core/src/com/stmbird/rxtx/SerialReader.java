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
        String DIVIDE;
        int len = -1;

        try {
            Robot robot = new Robot();

            while ((len = this.in.read(buffer)) > -1) {
                READ = new String(buffer,0,len);

                if(READ.length()>0)
                {
                    //DIVIDE = READ.substring(1,2);
                    //if(DIVIDE.substring(0, 1).equals("a"))
                    {
                        System.out.println(READ);

                        System.out.println("UP!");
                        robot.keyPress(KeyEvent.VK_SPACE);
                        robot.keyRelease(KeyEvent.VK_SPACE);
                        //try {
                           // Thread.sleep(550);                 //1000 milliseconds is one second.
                       // } catch(InterruptedException ex) {
                           // Thread.currentThread().interrupt();
                        //}

                        //robot.mousePress(InputEvent.BUTTON1_MASK);
                        //robot.mouseRelease(InputEvent.BUTTON1_MASK);

                    }

                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public void isConnected(){

    }
}
