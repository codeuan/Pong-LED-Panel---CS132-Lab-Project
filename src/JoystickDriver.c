/*Converts joystick movements into digital readings via ADC.*/
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include "JoystickDriver.h"

void joystickInitPins(void)
{
    rcc_periph_clock_enable(RCC_GPIOA); //enable the clock for port A, so we can send prompts to it.
    rcc_periph_clock_enable(RCC_GPIOC);  //enable the clock for port C, so we can send prompts to it.
    
    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0 | GPIO1); //set up pin 0 on port A.
    gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0 | GPIO1); //set up pin 0 on port C.
}

void joystickInitADC(void)
{
    rcc_periph_clock_enable(RCC_ADC12); //enable clock for ADC, on ADC1 and ADC2 which share one peripheral clock.
    adc_set_clk_prescale(ADC1, ADC_CCR_CKMODE_DIV1); //ADC clock uses bus clock at full speed.
    
    adc_enable_regulator(ADC1);
    adc_enable_regulator(ADC2); //enable the internal voltage regulator to give the ADC a stable supply.
    for (volatile unsigned int i = 0; i < 1000; ++i) { //allow at least 10 microseconds for both regulators to settle.
    }
  
    adc_calibrate(ADC1); 
    adc_calibrate(ADC2); //find offset at 0V and calibrate accordingly.
  
    uint8_t joystickAChannels[] = {1};  // PA0.
    uint8_t joystickBChannels[] = {6};  // PC0.

    adc_set_regular_sequence(ADC1, 1, joystickAChannels); //configure input pins for ADC1. Each channel is one axis.
    adc_set_regular_sequence(ADC2, 1, joystickBChannels); //configure input pins for ADC2. Each channel is one axis.
  
    adc_power_on(ADC1);
    adc_power_on(ADC2); //enable ADC ready for input.
}



uint16_t joystickRead(uint32_t adc)
{
    uint16_t reading; //only one axis matters. 

    adc_start_conversion_regular(adc); //start converting voltage.

    while (!adc_eoc(adc)) {
        //wait for the first axis to be ready.
    }
    reading = adc_read_regular(adc); //take reading.

    return reading; //return reading to main program
}


float joystick_position(uint32_t value) { 
    /*Normalise the reading between 0 and 1.*/
    int JOY_BASE = 2110; // Assuming a 12-bit ADC with a midpoint of 2048
    int DEAD_ZONE = 100; // Define the dead zone threshold
    int JOY_MIN = 1120;
    int JOY_MAX = 3120; // Assuming a 12-bit ADC with a maximum of 4095
    
    //ignore small movements around the centre 
    if (value >= JOY_BASE - DEAD_ZONE && 
        value <= JOY_BASE + DEAD_ZONE) { 
        return 0.5f; 
    } 
 
    float result; 
 
    if (value < JOY_BASE) { 
 
        //lower half of the joystick range 
        result = 0.5f * ((float)value - JOY_MIN)
                       / (JOY_BASE - JOY_MIN); 
 
    } else { 
 
        //upper half of the joystick range 
        result = 0.5f + 0.5f * (float)(value - JOY_BASE) 
                              / (float)(JOY_MAX - JOY_BASE); 
    } 
 
    //clamp the result 
    if (result < 0.0f) result = 0.0f; 
    if (result > 1.0f) result = 1.0f; 
 
    return result; 
}