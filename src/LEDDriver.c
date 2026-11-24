#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define WIDTH 32
#define HEIGHT 32

#define OFF   0
#define RED   1
#define BLUE  2
#define GREEN 4 //"define" means replace all instances of GREEN with 4 BEFORE compilation.

uint8_t framebuffer[HEIGHT][WIDTH] = {0}; //create empty array to store display. convention is [y][x].

void pulseBit(uint8_t bit)
{
  /*send a binary bit into LED panel's shift register
  when we select row n we also select row n + 16.
  */
  
    // Start with clock LOW
    gpio_clear(GPIOC, GPIO7);

    // Set data pin to the desired bit
    if (bit == 1) {
        gpio_set(GPIOC, GPIO6);
    } else {
        gpio_clear(GPIOC, GPIO6);
    }

    // Clock HIGH: shift the bit into the register
    gpio_set(GPIOC, GPIO7);
}



void send32Bits(uint32_t data)
{
  /*send 32 bit integer into LED panel shift register*/
    for (int i = 31; i >= 0; i--) {
        uint8_t bit = (data >> i) & 1; //find out what the bit at position i should be.
        pulseBit(bit); //send it.
    }
}


void selectRow(uint8_t row)
{
    /* Select which pair of rows to update.
      Pins used are 02, 03, 04, 05
    */
  
    // Only allow rows 0-15
    row &= 0x0F;

    // Clear the existing row selection
    gpio_clear(GPIOC, GPIO2 | GPIO3 | GPIO4 | GPIO5);

    // Set the appropriate pins (move into positions 2-5).
    gpio_set(GPIOC, (row << 2));
}



void sendRowPair(uint8_t row,
                 uint32_t topRed,
                 uint32_t topBlue,
                 uint32_t topGreen,
                 uint32_t bottomRed,
                 uint32_t bottomBlue,
                 uint32_t bottomGreen)
{
    //select the row pair
    selectRow(row);

    //keep the old display visible while updating
    gpio_set(GPIOC, GPIO8);

    // Send the furthest registers first
    send32Bits(bottomGreen);
    send32Bits(bottomBlue);
    send32Bits(bottomRed);

    send32Bits(topGreen);
    send32Bits(topBlue);
    send32Bits(topRed);

    //the latch is active-low, we need to clear it first. 
    gpio_clear(GPIOC, GPIO8);
    gpio_set(GPIOC, GPIO8);
}

void setPixel(int x, int y, uint8_t colour)
{
    /*select coordinates of pixel to be changed.*/
  
    //ignore coordinates outside the display
    if (x < 0 || x >= WIDTH ||
        y < 0 || y >= HEIGHT) {
        return;
    }

    //store the desired colour
    framebuffer[y][x] = colour & 0x07;
}


void refreshDisplay(void)
{
    /*refresh display according to framebuffer. */
    //iterate through the 16 row pairs
    for (int row = 0; row < 16; row++) {

        uint32_t topRed = 0, topBlue = 0, topGreen = 0;
        uint32_t bottomRed = 0, bottomBlue = 0, bottomGreen = 0;

        //examine all 32 columns
        for (int x = 0; x < 32; x++) {

            //find the bit representing this column
            uint32_t mask = 1U << (31 - x);

            //read colours from our framebuffer
            uint8_t top = framebuffer[row][x];
            uint8_t bottom = framebuffer[row + 16][x];

            //construct the top row's colour registers
            if (top & RED)   topRed |= mask; //the OR operator preserves existing bits.
            if (top & BLUE)  topBlue |= mask;
            if (top & GREEN) topGreen |= mask;

            //construct the bottom row's colour registers
            if (bottom & RED)   bottomRed |= mask;
            if (bottom & BLUE)  bottomBlue |= mask;
            if (bottom & GREEN) bottomGreen |= mask;
        }

        //transmit all six registers.
        sendRowPair(row,
                    topRed, topBlue, topGreen,
                    bottomRed, bottomBlue, bottomGreen);
    }
}
