
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "led_driver.h"

// Initialise the LED panel's GPIO connections
void initialiseLEDPanel(void)
{
    // Enable clock for GPIO Port C
    rcc_periph_clock_enable(RCC_GPIOC);

    // Configure PC2-PC8 as digital outputs
    gpio_mode_setup(
        GPIOC,
        GPIO_MODE_OUTPUT,
        GPIO_PUPD_NONE,
        GPIO2 | GPIO3 | GPIO4 | GPIO5 |
        GPIO6 | GPIO7 | GPIO8
    );

    // Configure outputs as push-pull
    gpio_set_output_options(
        GPIOC,
        GPIO_OTYPE_PP,
        GPIO_OSPEED_100MHZ,
        GPIO2 | GPIO3 | GPIO4 | GPIO5 |
        GPIO6 | GPIO7 | GPIO8
    );

    // Initially select rows 0 and 16
    gpio_clear(GPIOC, GPIO2 | GPIO3 | GPIO4 | GPIO5);

    // Initialise data and clock LOW
    gpio_clear(GPIOC, GPIO6 | GPIO7);

    // Start with latch HIGH
    gpio_set(GPIOC, GPIO8);
}

int main(void)
{
    //Configure the hardware
    initialiseLEDPanel();

    //Send the framebuffer to the display
    refreshDisplay();

    //Main loop
    while (1) {
        // Pong logic will go here later
    }

    return 0;
}
