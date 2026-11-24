
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "LEDDriver.h"
#include "JoystickDriver.h"
#include <stdio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/usart.h>

uint16_t readJoystickA;
uint16_t readJoystickB;
float normalisedReadJoystickA;
float normalisedReadJoystickB;

int main(void)
{
    initialiseLEDPanel(); //configure the LED panel.
    gameInit(); //draw the initial paddles on the display.

    refreshDisplay(); //send the framebuffer to the display

    joystickInitPins();
    joystickInitADC(); //configure the joysticks.

    //Main loop
    while (1) {
        gameUpdate();
    }

    return 0;
}
