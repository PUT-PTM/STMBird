package com.stmbird.rxtx;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

import java.io.InputStream;

/**
 * Class that allows us to find and connect our STM32 Controller.
 */

public class STMHandler
{

    SerialPort serialPort;

    public STMHandler() {
        super();
    }

    public void connect(String portName) throws Exception {
        CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
        if (portIdentifier.isCurrentlyOwned()) {
            System.out.println("Error: Port is currently in use");
        } else {

            CommPort commPort = portIdentifier.open(this.getClass().getName(), 2000);

            if (commPort instanceof SerialPort) {
                SerialPort serialPort = (SerialPort) commPort;
                serialPort.setSerialPortParams(57600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);

                InputStream in = serialPort.getInputStream();
                //In our case we are not using SerialWriter
              //OutputStream out = serialPort.getOutputStream();

                (new Thread(new SerialReader(in))).start();
                //In our case we are not using SerialWriter
              //(new Thread(new SerialWriter(out))).start();

            } else {
                System.out.println("Error: Only serial ports are handled.");
            }
        }
    }

    public void disconnect() throws Exception {

        try
        {
            if (serialPort != null)
            {
                serialPort.removeEventListener();
                serialPort.close();
                serialPort.getInputStream().close();
                serialPort.getOutputStream().close();

            }

        } catch (Exception e)
        {
            e.printStackTrace();

        }
    }

}

