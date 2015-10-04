package com.stmbird.gameobjects;

/**
 * Class that contains information about our Grass it's postion.
 */
public class Grass extends Scrollable
{

    public Grass(float x, float y, int width, int height, float scrollSpeed) {
        super(x, y, width, height, scrollSpeed);

    }

    public void onRestart(float x, float scrollSpeed) {
        position.x = x;
        velocity.x = scrollSpeed;
    }
}
