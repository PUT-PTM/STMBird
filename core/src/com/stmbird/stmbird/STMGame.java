package com.stmbird.stmbird;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.stmbird.helpers.AssetLoader;
import com.stmbird.screens.GameScreen;


public class STMGame extends Game {

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
    }

}
