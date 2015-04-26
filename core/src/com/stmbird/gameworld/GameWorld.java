package com.stmbird.gameworld;

import com.badlogic.gdx.math.Intersector;
import com.badlogic.gdx.math.Rectangle;
import com.stmbird.gameobjects.Bird;
import com.stmbird.gameobjects.ScrollHandler;



public class GameWorld {

    private Bird bird;
    private ScrollHandler scroller;
    private Rectangle ground;

    public GameWorld(int midPointY) {
        bird = new Bird(33, midPointY - 5, 17, 12);
        scroller = new ScrollHandler(midPointY + 66);
        ground = new Rectangle(0, midPointY + 66, 136, 11);

    }



    public void update(float delta) {

        if (delta > .15f) {
            delta = .15f;
        }

        bird.update(delta);
        scroller.update(delta);

        if (scroller.collides(bird) && bird.isAlive()) {
            scroller.stop();
            bird.die();

        }

        if (Intersector.overlaps(bird.getBoundingCircle(), ground)) {
            scroller.stop();
            bird.die();
            bird.decelerate();
        }

    }

    public Bird getBird() {
        return bird;

    }

    public ScrollHandler getScroller() {
        return scroller;
    }

}
