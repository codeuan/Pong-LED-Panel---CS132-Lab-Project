#include <libopencm3/stm32/adc.h>
#include "Game.h"
#include "JoystickDriver.h"
#include "LEDDriver.h"

#define PADDLE_HEIGHT 4
#define MAX_PADDLE_Y  (HEIGHT - PADDLE_HEIGHT)
#define LEFT_X        0
#define RIGHT_X       (WIDTH - 1)
#define PADDLE_STEP_FRAMES 8
static int leftY = MAX_PADDLE_Y / 2;
static int rightY = MAX_PADDLE_Y / 2;



static void drawPaddle(int x, int y, uint8_t colour)
{
    for (int offset = 0; offset < PADDLE_HEIGHT; ++offset) {
        setPixel(x, y + offset, colour); //keep x fixed but draw the paddle from top to bottom.
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
    drawPaddle(LEFT_X, leftY, RED);
    drawPaddle(RIGHT_X, rightY, BLUE);
}
