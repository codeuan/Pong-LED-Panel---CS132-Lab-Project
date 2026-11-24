#ifndef GAME_H
#define GAME_H

void gameInit(void);
void gameUpdate(void);
static int updatePaddle(int x, int y, uint8_t colour, uint32_t adc);
static void drawPaddle(int x, int y, uint8_t colour);
#endif