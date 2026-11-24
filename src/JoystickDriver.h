#ifndef JOYSTICK_DRIVER_H
#define JOYSTICK_DRIVER_H

#include <stdint.h>

void joystickInitPins(void);
void joystickInitADC(void);
uint16_t joystickRead(uint32_t joystick);
float joystick_position(uint32_t value);
#endif