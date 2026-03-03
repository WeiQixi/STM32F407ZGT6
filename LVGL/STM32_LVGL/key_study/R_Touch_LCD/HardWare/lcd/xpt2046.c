#include "xpt2046.h"
#include "DWT_delay.h"

uint16_t XDown=0;
uint16_t YDown=0;
uint16_t XUp=0;
uint16_t YUp=0;

uint8_t CMD_RDX=0xD0;
uint8_t CMD_RDY=0x90;

float xFactor=0.06671114;	  // 竖屏校准参数 
float yFactor=0.09117551;
short xOffset=-11;          // xOffset和yOffset 可以根据实际手头的电阻屏适当调整到精准
short yOffset=-18;

/**************以下为普通IO模拟SPI通信XPT2046***************/
/****************************************************************************
* 名    称: void SPI_Write_Byte(u8 num) 
* 功    能：SPI写数据,向触摸屏IC写入1byte数据 
* 入口参数：num:要写入的数据
* 返回参数：无
* 说    明：       
****************************************************************************/
void SPI_Write_Byte(uint8_t num){
  uint8_t count=0;
  for(count=0;count<8;count++){
    if(num&0x80) TDIN=1;
    else TDIN=0;
    num<<=1;
    TCLK=0;
    DWT_Delay_us(1);
    TCLK=1;
  }
}

/****************************************************************************
* 名    称: u16 SPI_Read_AD(u8 CMD)
* 功    能：SPI读数据 ,从触摸屏IC读取adc值
* 入口参数：CMD:指令
* 返回参数: 读到的数据	   
* 说    明：       
****************************************************************************/ 
uint16_t SPI_Read_AD(uint8_t CMD){
  uint8_t count=0;
  uint16_t num=0;
  TCLK=0;   //先拉低时钟
  TDIN=0;   //拉低数据线
  TCS=0;    //选中触摸屏IC 2046
  SPI_Write_Byte(CMD);
  DWT_Delay_us(6);
  TCLK=0;
  DWT_Delay_us(1);
  TCLK=1;
  DWT_Delay_us(1);
  TCLK=0;
  for(count=0;count<16;count++){
    num<<=1;
    TCLK=0;
    DWT_Delay_us(1);
    TCLK=1;
    if(DOUT) num++;
  }
  num>>=4;
  TCS=1;
  return num;
}
/**************以上为普通IO模拟SPI通信XPT2046***************/

/****************************************************************************
* 名    称: u16 RTouch_Read_XorY(u8 xy)
* 功    能：读取一个坐标值(x或者y)
* 入口参数：xoy:指令（CMD_RDX/CMD_RDY
* 返回参数: 读到的数据	   
* 说    明：连续读取5次数据,对这些数据升序排列, 然后去掉最低和最1个数,取平均值       
****************************************************************************/
uint16_t RTouch_Read_XorY(uint8_t xy){
  uint16_t i,j;
  uint16_t buf[5];
  uint16_t sum=0;
  uint16_t temp;

  for(i=0;i<5;i++) buf[i]=SPI_Read_AD(xy);
  for(i=0;i<5-1;i++){
    for(j=i+1;j<5;j++){
      if(buf[i]>buf[j]){
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  sum=0;
  for(i=1;i<5-1;i++) sum+=buf[i];
  temp=sum/(5-2*1);
  return temp;
}

/****************************************************************************
* 名    称: u8 RTouch_Read_XY(u16 *x,u16 *y)
* 功    能：读取x,y坐标
* 入口参数：x,y:读取到的坐标值
* 返回参数: 返回值:0,失败;1,成功   
* 说    明：最小值不能少于50.       
****************************************************************************/
uint8_t RTouch_Read_XY(uint16_t* x,uint16_t* y){
  uint16_t xtemp,ytemp;
  xtemp=RTouch_Read_XorY(CMD_RDX);
  ytemp=RTouch_Read_XorY(CMD_RDY);
  if(xtemp<50||ytemp<50) return 0;
  *x=xtemp;
  *y=ytemp;
  return 1;
}

/****************************************************************************
* 名    称: u8 RTouch_Read_XY2(u16 *x,u16 *y)
* 功    能：连续2次读取触摸屏IC
* 入口参数：x,y:读取到的坐标值
* 返回参数: 返回值:0,失败;1,成功   
* 说    明：        
****************************************************************************/
uint8_t RTouch_Read_XY2(uint16_t* x,uint16_t* y){
  uint16_t x1,y1;
  uint16_t x2,y2;
  uint8_t flag;
  flag=RTouch_Read_XY(&x1,&y1);
  if(flag==0) return 0;
  flag=RTouch_Read_XY(&x2,&y2);
  if(flag==0) return 0;
  if (((x2 <= x1 && x1 < x2 + 50) || (x1 <= x2 && x2 < x1 + 50)) // 前后两次采样在+-50内
      && ((y2 <= y1 && y1 < y2 + 50) || (y1 <= y2 && y2 < y1 + 50)))
  {
    *x = (x1 + x2) / 2;
    *y = (y1 + y2) / 2;
    return 1;
  }
  else
    return 0;
}

uint16_t x;
uint16_t y;
uint8_t time;
/****************************************************************************
* 名    称: void XPT2046_Scan(u8 tp)
* 功    能：触摸按键扫描	
* 入口参数：type:0,屏幕坐标;1,物理坐标 
* 返回参数: 无  
* 说    明：   启￥明#欣￥欣     
****************************************************************************/
void XPT2046_Scan(uint8_t type){
  XUp=0xFFFF;
  YUp=0xFFFF;
  if(PEN==0){     // 有按键按下
    if(type) RTouch_Read_XY2(&x,&y);    // 读取物理坐标
    else if(RTouch_Read_XY2(&x,&y)){    // 读取屏幕坐标
      x=xFactor*x+xOffset;
      y=yFactor*y+yOffset;
    }
    XDown=x;
    YDown=y;
    time++;
  }else{  // 键抬起
    if(time>2){
      XUp=x;
      YUp=y;
    }
    time=0;
    XDown=0xFFFF;
    YDown=0xFFFF;
  }
}

void Soft_SPI_GPIO_Init(void){
  GPIO_InitTypeDef GPIO_Init;
  // __HAL_RCC_GPIOA_CLK_ENABLE(); // gpio.c开过
  __HAL_RCC_GPIOB_CLK_ENABLE();
  // __HAL_RCC_GPIOF_CLK_ENABLE();  // gpio.c开过
  GPIO_Init.Pin=GPIO_PIN_5;
  GPIO_Init.Mode=GPIO_MODE_INPUT;
  GPIO_Init.Pull=GPIO_PULLUP;
  GPIO_Init.Speed=GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOB,&GPIO_Init);

  GPIO_Init.Pin=GPIO_PIN_11;
  HAL_GPIO_Init(GPIOF,&GPIO_Init);

  GPIO_Init.Pin=GPIO_PIN_0|GPIO_PIN_1;
  GPIO_Init.Mode=GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOB,&GPIO_Init);

  GPIO_Init.Pin=GPIO_PIN_5;
  HAL_GPIO_Init(GPIOA,&GPIO_Init);
}

void XPT2046_Init(void){
  Soft_SPI_GPIO_Init();
  if(dir_flag){
    CMD_RDX=0x90;
    CMD_RDY=0xD0;
    xFactor=-0.09195402;	// 横屏校准参数 
    yFactor=0.06736275;
    xOffset=348;
    yOffset=-19;
  }else{
    CMD_RDX=0XD0;
		CMD_RDY=0X90;
    xFactor=0.06671114;	  // 竖屏校准参数 
    yFactor=0.09117551;
    xOffset=-11;  
    yOffset=-18;
  }
}
