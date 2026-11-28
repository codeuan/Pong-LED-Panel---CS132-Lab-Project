#include <libopencm3/stm32/adc.h>
#include <time.h>
#include "Game.h"
#include "JoystickDriver.h"
#include "LEDDriver.h"

#define PADDLE_HEIGHT 4
#define MAX_PADDLE_Y  (HEIGHT - PADDLE_HEIGHT)

#define LEFT_X        0
#define RIGHT_X       (WIDTH - 1)
#define PADDLE_STEP_FRAMES 8

#define BALL_HEIGHT 1
#define BALL_WIDTH 1
#define MAX_BALL_HEIGHT  (HEIGHT - BALL_HEIGHT)
#define MAX_BALL_X (WIDTH - BALL_WIDTH)

struct Ball {
    int x;
    int y;
    int vel_x;
    int vel_y;
}

static int leftY = MAX_PADDLE_Y / 2;
static int rightY = MAX_PADDLE_Y / 2;

int nextXPosition;

static void drawPaddle(int x, int y, uint8_t colour)
{
    for (int offset = 0; offset < PADDLE_HEIGHT; ++offset) {
        setPixel(x, y + offset, colour); //keep x fixed but draw the paddle from top to bottom.
    }
}

static void drawBall(Ball ball, uint8_t colour)
{
    for (int y_offset = 0; x_offset < BALL_HEIGHT; ++offset) {
        for (int x_offset = 0; x_offset < BALL_WIDTH; ++offset) {
            setPixel(ball.x + x_offset, ball.y + y_offset, colour);
        }     

    }
}

static int updatePaddle(int x, int y, uint8_t colour, uint32_t adc)
{
    float position = joystick_position(joystickRead(adc));

    if (position == 0.5f) {
        return y; // Joystick is in the dead zone. Keep the current row.
    }

    int targetY = (int)(position * MAX_PADDLE_Y + 0.5f);

    if (targetY == y) {
        return y;
    }

    drawPaddle(x, y, OFF);

    if (targetY > y) {
        y++; //move paddle one down.
    } else {
        y--; //move paddle one up.
    }

    drawPaddle(x, y, colour);
    return y;
}

static Ball updateBall(Ball ball, int leftY, int rightY, uint8_t colour)
{
    drawBall(ball, OFF);
    
    if ((ball.y >= MAX_BALL_HEIGHT) || (ball.y <= 0)) {
        ball.vel_y *= -1;
        ball.x += ball.vel_x;
        ball.y += ball.vel_y;
    }

    else if ((ball.x >= MAX_BALL_X) || (ball.x <= 0)) {
        ball.vel_x *= -1;
        ball.x += ball.vel_x;
        ball.y += ball.vel_y;
    }

    else if ((ball.x == LEFT_X) && ((ball.y - leftY) < PADDLE_HEIGHT) && ((ball.y - leftY) >= 0)) {
        ball.vel_x *= -1;
        ball.x += ball.vel_x;
        ball.y += ball.vel_y;
    }

    else if ((ball.x == RIGHT_X) && ((ball.y - rightY) < PADDLE_HEIGHT) && ((ball.y - rightY) >= 0)) {
        ball.vel_x *= -1;
        ball.x += ball.vel_x;
        ball.y += ball.vel_y;
    }
    else {
        ball.x += ball.vel_x;
        ball.y += ball.vel_y;

    }

    drawBall(ball, colour);

    return ball;


    
}

void gameUpdate(void)
{
    static unsigned frames = 0;

    refreshDisplay();
    frames++;

    if (frames >= PADDLE_STEP_FRAMES) {
        frames = 0;
        leftY = updatePaddle(LEFT_X, leftY, RED, ADC1);
        rightY = updatePaddle(RIGHT_X, rightY, BLUE, ADC2);
    }
}

void gameInit(void)
{
    srand(time(NULL));
    drawPaddle(LEFT_X, leftY, RED);
    drawPaddle(RIGHT_X, rightY, BLUE);

    static int starting_X_velocities[] = {-1, 1};
    static int starting_Y_velocities[] = {-1, 1};

    struct Ball ball;

    ball.x = MAX_BALL_X/2;
    ball.y = MAX_BALL_Y/2;

    ball.vel_x = starting_X_velocities[rand() % 2];
    ball.vel_y = starting_Y_velocities[rand() % 2];

    nextXPosition = ball.x + ball.vel_x;
    if nextXPosition < 0 { //ball has overshot the left edge, player two scores.
        OutOfBounds(2);
    } else if (nextXPosition > 31) {//ball has overshot the right edge, player one scores.
        OutOfBounds(1);
    }
}
