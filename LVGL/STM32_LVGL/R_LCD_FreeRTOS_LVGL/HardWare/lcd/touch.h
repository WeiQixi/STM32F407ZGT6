#ifndef __TOUCH_H
#define __TOUCH_H

#include "main.h"
#include "xpt2046.h"

void Touch_Init(void);
void Clear_Screen(void);
void Draw_Point(uint16_t x,uint16_t y,uint16_t color);
void R_Touch_test(void* pvParameters);

#endif
