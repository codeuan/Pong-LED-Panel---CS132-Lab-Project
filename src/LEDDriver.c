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

void pulseBit(uint8_t bit)
{
  /*send a binary bit into LED panel's shift register
  when we select row n we also select row n + 16.
  */
  
    //start with clock LOW
    gpio_clear(GPIOC, GPIO7);

    //set data pin to the desired bit
    if (bit == 1) {
        gpio_set(GPIOC, GPIO6);
    } else {
        gpio_clear(GPIOC, GPIO6);
    }

    //clock HIGH: shift the bit into the register
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
    /*select which pair of rows to update.
      Pins used are 02, 03, 04, 05
    */
  
    //only allow rows 0-15
    row &= 0x0F;

    //clear the existing row selection
    gpio_clear(GPIOC, GPIO2 | GPIO3 | GPIO4 | GPIO5);

    //set the appropriate pins (move into positions 2-5).
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

 void setPixel(int x, int y, uint32_t colour)
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


void drawWin(int x, int y, uint8_t colour) { //set by upper leftmost pixel.
    // W
    setPixel(x, y, colour);
    setPixel(x + 2, y, colour);
    setPixel(x + 4, y, colour);

    setPixel(x, y + 1, colour);
    setPixel(x + 2, y + 1, colour);
    setPixel(x + 4, y + 1, colour);

    setPixel(x, y + 2, colour);
    setPixel(x + 1, y + 2, colour);
    setPixel(x + 2, y + 2, colour);
    setPixel(x + 3, y + 2, colour);
    setPixel(x + 4, y + 2, colour);

    // I
    setPixel(x + 7, y, colour);
    setPixel(x + 7, y + 1,colour);
    setPixel(x + 7, y + 2,colour);
    
    // N
    setPixel(x + 9,  y, colour);
    setPixel(x + 10, y, colour);
    setPixel(x + 11, y, colour);
    
    setPixel(x + 9, y + 1, colour);
    setPixel(x + 10, y + 1, colour);
    setPixel(x + 11, y + 1,colour);
    
    setPixel(x + 9,  y + 2, colour);
    setPixel(x + 11, y + 2, colour);
}

void drawTallies(int x, int y, int score, uint8_t colour) {
    for (int i = 0; i < score; i++) {
        for (int j = 0; j < 5; j++) {
            setPixel(x + 2 * i, y + j, colour); //tallies are 2 pixels apart.
        }
    }
}
