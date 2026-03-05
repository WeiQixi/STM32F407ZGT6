#ifndef __LCD_H
#define __LCD_H

#include "main.h"
#include "fsmc.h"
#include "cfont.h"

#define BackLight_Pin  GPIO_PIN_10
#define BackLight_GPIO_Port GPIOF
#define BackLight PFout(10)

// A12作为数据命令区分线  设置时STM32内部会右移wei会右移一位对齐
#define CMD_BASE ((uint32_t)(0x6C000000|0x00001FFE))
#define DAT_BASE ((uint32_t)(0x6C000000|0x00002000))

#define LCD_CMD (*(volatile uint16_t*)CMD_BASE)
#define LCD_DAT (*(volatile uint16_t*)DAT_BASE)

// 扫描方向定义
#define L2R_U2D 0 // 从左到右，从上到下
#define L2R_D2U 1 // 从左到右，从下到上
#define R2L_U2D 2 // 从右到左，从上到下
#define R2L_D2U 3 // 从右到左，从下到上

#define U2D_L2R 4 // 从上到下，从左到右
#define U2D_R2L 5 // 从上到下，从右到左
#define D2U_L2R 6 // 从下到上，从左到右
#define D2U_R2L 7 // 从下到上，从右到左

//颜色值定义
#define  WHITE        0xFFFF
#define  BLACK        0x0000	
#define  RED          0xF800
#define  GREEN        0x07E0
#define  BLUE         0x001F 
#define  BRED         0XF81F
#define  GRED 			  0XFFE0
#define  GBLUE			  0X07FF
#define  BROWN 			  0XBC40  
#define  BRRED 			  0XFC07  
#define  GRAY  			  0X8430  
#define  MAGENTA      0xF81F
#define  CYAN         0x7FFF
#define  YELLOW       0xFFE0

extern uint8_t dir_flag;

//LCD的画笔颜色和背景色	   
extern uint16_t  BRUSH_COLOR;//默认红色    
extern uint16_t  BACK_COLOR; //背景颜色.默认为白色

extern uint16_t lcd_width;       // LCD的宽度
extern uint16_t lcd_height;      // LCD的高度

void LCD_Init(void);

void LCD_Open_Window(uint16_t X0,uint16_t Y0, uint16_t width, uint16_t height);
void Set_Scan_Direction(uint8_t direction);
void Set_Display_Mode(uint8_t mode);

void LCD_DisplayON(void);
void LCD_DisplayOFF(void);
void LCD_SetCursor(uint16_t Xaddr, uint16_t Yaddr);
void LCD_Clear(uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y);
void LCD_Color_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);

void LCD_Fill_onecolor(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
void LCD_Draw_Picture(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t* color);
void LCD_DisplayChar(uint16_t x,uint16_t y,uint8_t word,uint8_t size);
void LCD_DisplayString(uint16_t x,uint16_t y,uint8_t size,uint8_t* p);
void LCD_DisplayString_color(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint16_t brushcolor,uint16_t backcolor);
void LCD_DisplayNum(uint16_t x, uint16_t y, uint32_t value, uint8_t len, uint8_t size, uint8_t mode);
void LCD_DisplayNum_color(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode,uint16_t brushcolor,uint16_t backcolor);

void LCD_task(void* pvParameters);

#endif
