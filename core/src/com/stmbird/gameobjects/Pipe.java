package com.stmbird.gameobjects;

import com.badlogic.gdx.math.Intersector;
import com.badlogic.gdx.math.Rectangle;
import java.util.Random;

/**
 * Class that contains information about our Pipes, it's postions, width, height.
 */

public class Pipe extends Scrollable
{

    private Random r;

    private Rectangle HeadUp, HeadDown, barUp, barDown;

    public static final int VERTICAL_GAP = 45;
    public static final int Head_WIDTH = 24;
    public static final int Head_HEIGHT = 11;

    private float groundY;

    private boolean isScored = false;

    public Pipe(float x, float y, int width, int height, float scrollSpeed,
                float groundY) {
        super(x, y, width, height, scrollSpeed);
        // Initialize a Random object for Random number generation
        r = new Random();
        HeadUp = new Rectangle();
        HeadDown = new Rectangle();
        barUp = new Rectangle();
        barDown = new Rectangle();

        this.groundY = groundY;
    }

    @Override
    public void update(float delta) {
        // Call the update method in the superclass -Scrollable
        super.update(delta);



        barUp.set(position.x, position.y, width, height);
        barDown.set(position.x, position.y + height + VERTICAL_GAP, width,
                groundY - (position.y + height + VERTICAL_GAP));



        HeadUp.set(position.x - (Head_WIDTH - width) / 2, position.y + height
                - Head_HEIGHT, Head_WIDTH, Head_HEIGHT);
        HeadDown.set(position.x - (Head_WIDTH - width) / 2, barDown.y,
                Head_WIDTH, Head_HEIGHT);

    }

    @Override
    public void reset(float newX) {
        // Call the reset method in the superclass -scrollable
        super.reset(newX);
        // Change the height to a random number
        //height = r.nextInt(90) + 15;
        height = r.nextInt(90) + 15;
        isScored = false;

    }

    public Rectangle getHeadUp() {
        return HeadUp;
    }

    public Rectangle getHeadDown() {
        return HeadDown;
    }

    public Rectangle getBarUp() {
        return barUp;
    }

    public Rectangle getBarDown() {
        return barDown;
    }

    public boolean collides(Bird bird) {
        if (position.x < bird.getX() + bird.getWidth()) {
            return (Intersector.overlaps(bird.getBoundingCircle(), barUp)
                    || Intersector.overlaps(bird.getBoundingCircle(), barDown)
                    || Intersector.overlaps(bird.getBoundingCircle(), HeadUp) || Intersector
                    .overlaps(bird.getBoundingCircle(), HeadDown));
        }
        return false;
    }

    public boolean isScored() {
        return isScored;
    }

    public void setScored(boolean b) {
        isScored = b;
    }

    public void onRestart(float x, float scrollSpeed) {
        velocity.x = scrollSpeed;
        reset(x);
    }
}
