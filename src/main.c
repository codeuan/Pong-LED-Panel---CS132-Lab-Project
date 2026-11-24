
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "led_driver.h"

void initialiseLEDPanel(void)
{ //initialise the LED panel's GPIO connections
    rcc_periph_clock_enable(RCC_GPIOC); //enable clock for GPIO Port C

    gpio_mode_setup(
        GPIOC,
        GPIO_MODE_OUTPUT,
        GPIO_PUPD_NONE,
        GPIO2 | GPIO3 | GPIO4 | GPIO5 |
        GPIO6 | GPIO7 | GPIO8
    ); //configure PC2-PC8 as digital outputs

    gpio_set_output_options(
        GPIOC,
        GPIO_OTYPE_PP,
        GPIO_OSPEED_100MHZ,
        GPIO2 | GPIO3 | GPIO4 | GPIO5 |
        GPIO6 | GPIO7 | GPIO8
    ); //configure outputs as push-pull

    gpio_clear(GPIOC, GPIO2 | GPIO3 | GPIO4 | GPIO5);     //initially select rows 0 and 16

    gpio_clear(GPIOC, GPIO6 | GPIO7); //initialise data and clock LOW

    gpio_set(GPIOC, GPIO8); //start with latch HIGH
}

int main(void)
{
    initialiseLEDPanel(); //configure the LED panel.

    refreshDisplay(); //send the framebuffer to the display

    joystickInitPins()
    joystickInitADC(); //configure the joysticks.

    //Main loop
    while (1) {
        // Pong logic will go here later
    }

    return 0;
}
