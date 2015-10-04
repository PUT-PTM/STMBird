package com.stmbird.desktop;

import com.badlogic.gdx.Files;
import com.badlogic.gdx.backends.lwjgl.LwjglApplication;
import com.badlogic.gdx.backends.lwjgl.LwjglApplicationConfiguration;
import com.stmbird.rxtx.STMHandler;
import com.stmbird.stmbird.STMGame;

/**
 * Main Class that create new Instance of our Game, set our screen resolution and allow us to connect to our STM32 Controller.
 */

public class DesktopLauncher {
    public static void main (String[] arg) {
        LwjglApplicationConfiguration.disableAudio = true;
        LwjglApplicationConfiguration config = new LwjglApplicationConfiguration();
        config.title = "STMBird";
        config.width = 272;
        config.height = 408;
        config.addIcon("data/icon.png", Files.FileType.Internal );
        new LwjglApplication(new STMGame(), config);

        try
        {
            (new STMHandler()).connect("COM3");

        }
        catch ( Exception e)
        {

            e.printStackTrace();
        }

    }
}
