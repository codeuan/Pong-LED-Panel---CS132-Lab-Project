#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdint.h>

#define WIDTH  32
#define HEIGHT 32

#define OFF   0
#define RED   1
#define BLUE  2
#define GREEN 4

void initialiseLEDPanel(void);
void pulseBit(uint8_t bit);
void send32Bits(uint32_t data);
void selectRow(uint8_t row);
void sendRowPair(uint8_t row,
                 uint32_t topRed,
                 uint32_t topBlue,
                 uint32_t topGreen,
                 uint32_t bottomRed,
                 uint32_t bottomBlue,
                 uint32_t bottomGreen);
void setPixel(int x, int y, uint8_t colour);
void refreshDisplay(void);

#endif