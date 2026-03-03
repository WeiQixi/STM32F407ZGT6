#ifndef __XPT2046_H
#define __XPT2046_H

#include "main.h"

// 电阻屏芯片连接引脚(软件SPI)
#define PEN   PFin(11)    // T_PEN
#define DOUT  PBin(2)     // T_MISO
#define TDIN  PBout(1)    // T_MOSI
#define TCLK  PAout(5)    // T_CLK
#define TCS   PBout(0)    // T_CS

// 触摸屏控制器参数
extern uint16_t XDown;    // 触摸屏按下就返回的坐标
extern uint16_t YDown;
extern uint16_t XUp;      // 触摸屏抬起就返回的坐标
extern uint16_t YUp;

extern float xFactor;     // 触摸屏校准参数
extern float yFactor;
extern short xOffset;
extern short yOffset;

//电阻屏函数
/*********电阻屏SPI通信与读取触摸点AD值*********************/
void SPI_Write_Byte(uint8_t num);                       // 向控制芯片写入一个数据
uint16_t SPI_Read_AD(uint8_t CMD);                      // 读取AD转换值
uint16_t RTouch_Read_XorY(uint8_t xy);                  // 带滤波的坐标读取(X/Y)
uint8_t RTouch_Read_XY(uint16_t* x,uint16_t* y);        // 双方向读取(X+Y)
uint8_t RTouch_Read_XY2(uint16_t* x,uint16_t* y);       // 两次读取的双方向坐标读取

/*********电阻屏初始化函数*********************/
void XPT2046_Init(void);

/*********电阻屏扫描按键函数*********************/
void XPT2046_Scan(uint8_t type);                        // tp:0,屏幕坐标;1,物理坐标

#endif
