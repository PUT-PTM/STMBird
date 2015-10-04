package com.stmbird.stmbird;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.stmbird.helpers.AssetLoader;
import com.stmbird.screens.GameScreen;
import gnu.io.SerialPort;

public class STMGame extends Game
{

    SerialPort serialPort;

    @Override
    public void create() {
        Gdx.app.log("STMGame", "created");
        AssetLoader.load();
        setScreen(new GameScreen());
    }

    @Override
    public void dispose() {
        super.dispose();
        AssetLoader.dispose();
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
