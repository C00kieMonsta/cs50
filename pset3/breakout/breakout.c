//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// bricks
#define BRICK_WIDTH 36
#define BRICK_HEIGHT 10

// paddle
#define PADDLE_WIDTH 3*BRICK_WIDTH
#define PADDLE_HEIGHT BRICK_HEIGHT

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// properties ball : from internet
#define BALL_DX     0.05
#define BALL_VEL    BALL_DX*BALL_DX*2
#define BALL_DY(dx) (sqrt(BALL_VEL - (dx*dx)))  


// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // velocity of the ball
    double velocityX = BALL_DX;
    double velocityY = BALL_DY(BALL_DX);
    
    waitForClick();
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        
        updateScoreboard(window,label, points);
        
        GEvent event1 = getNextEvent(MOUSE_EVENT);
        
        if (event1 != NULL)
        {
            if (getEventType(event1) == MOUSE_MOVED)
            {
                double x = getX(event1) - getWidth(paddle) / 2;
                
                setLocation(paddle, x, 550);
            }
        }
        
        GObject object = detectCollision(window, ball);
        
        move(ball, velocityX, velocityY); // (object, x-as, y-as)
        
        if (object != NULL)
        {
            if (object == paddle)
            {
                velocityY = -velocityY;
            }
            else if ((strcmp(getType(object), "GRect") == 0))
            {
                velocityY = -velocityY;
                removeGWindow(window,object);
                points ++;
                bricks--;
            }
        }
            
        if (getX(ball) + getWidth(ball) >= WIDTH)
        {
            velocityX = -velocityX;
        }
        else if (getX(ball) <= 0)
        {
            velocityX = -velocityX;
        }
        else if (getY(ball) + getHeight(ball) >= HEIGHT)
        {
            lives--;
            setLocation(ball, 190, 300);
            setLocation(paddle, 170, 550);
            waitForClick();
        }
    }
   
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    GRect bricks [ROWS][COLS];
    double y = 50.0;
    
    for (int i = 0; i < ROWS; i++)
    {
        double x = 2.0;
        for (int j = 0; j < COLS; j++)
        {
            bricks [i][j] = newGRect(x, y, BRICK_WIDTH, BRICK_HEIGHT);
            setFilled(bricks[i][j], true);
                if (i == 0)
                {
                    setColor(bricks[i][j], "RED");
                    add(window, bricks[i][j]);
                }
                else if (i == 1)
                {
                    setColor(bricks[i][j], "ORANGE");
                    add(window, bricks[i][j]);
                }
                else if (i == 2)
                {
                    setColor(bricks[i][j], "YELLOW");
                    add(window, bricks[i][j]);
                }
                else if (i == 3)
                {
                    setColor(bricks[i][j], "GREEN");
                    add(window, bricks[i][j]);
                }
                else
                {
                    setColor(bricks[i][j], "CYAN");
                    add(window, bricks[i][j]);
                }
            x = x + 40.0;
        }
        
        y = y + 14.0;
    }
    
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval circle = newGOval(190, 300, 2 * RADIUS, 2 * RADIUS);
    setColor(circle, "BLACK");
    setFilled(circle, true);
    add(window, circle);
    
    return circle;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(170, 550,PADDLE_WIDTH , PADDLE_HEIGHT);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("50");
    setFont(label, "SansSerif-36");
    setColor(label, "C0C0C0");
    add(window, label);
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;    
    setLocation(label, x, y);
    
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
