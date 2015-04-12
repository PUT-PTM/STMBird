package com.stmbird.desktop;

import com.badlogic.gdx.backends.lwjgl.LwjglApplication;
import com.badlogic.gdx.backends.lwjgl.LwjglApplicationConfiguration;
import com.stmbird.stmbird.STMGame;

public class DesktopLauncher {
    public static void main (String[] arg) {
        LwjglApplicationConfiguration.disableAudio = true;
        LwjglApplicationConfiguration config = new LwjglApplicationConfiguration();
        config.title = "STMBird";
        config.width = 272;
        config.height = 408;
        new LwjglApplication(new STMGame(), config);
    }
}
