#include "lcd.h"
#include "DWT_delay.h"

// 背光引脚
void Backlight_Pin_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct={0};
  /* GPIO Ports Clock Enable */
  /* gpio.c 已经打开过GPIO时钟 */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  GPIO_InitStruct.Pin = BackLight_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(BackLight_GPIO_Port,&GPIO_InitStruct);
}

// 初始化画笔颜色和背景色
uint16_t BRUSH_COLOR=BLACK;
uint16_t BACK_COLOR=WHITE;

// LCD驱动参数
uint16_t lcd_id;
uint8_t dir_flag;         // 横屏竖屏控制：0，竖屏；1，横屏
uint16_t lcd_width;       // LCD的宽度
uint16_t lcd_height;      // LCD的高度
uint16_t write_gramcmd;   // 写gram指令
uint16_t set_xcmd;        // 设置x坐标指令
uint16_t set_ycmd;        // 设置y坐标指令

uint16_t ILI9341_Read_id(void){
  uint16_t id;
  LCD_CMD=0xD3;     // 9341读ID命令
  id=LCD_DAT;
  id=LCD_DAT;       // 0x00
  id=LCD_DAT;       // 0x93
  id<<=8;
  id|=LCD_DAT;      // 0x41
  return id;
}

// 开始写GRAM
void LCD_WriteGRAM(void){
  LCD_CMD=write_gramcmd;
}

// 开启显示
void LCD_DisplayON(void){
  LCD_CMD=0x29;  // 9341与1963开显示命令一样
}
// 关闭显示
void LCD_DisplayOFF(void){
  LCD_CMD=0x28;  // 9341与1963关显示命令一样
}

/****************************************************************************
* 名    称: void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value)
* 功    能：LCD写寄存器
* 入口参数：LCD_Reg: 寄存器地址
*           LCD_RegValue: 要写入的数据
* 返回参数：无
* 说    明：       
****************************************************************************/	
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_Value){
  LCD_CMD=LCD_Reg;
  LCD_DAT=LCD_Value;
}

/****************************************************************************
* 名    称: u16 LCD_ReadReg(u16 LCD_Reg)
* 功    能：LCD读寄存器
* 入口参数：LCD_Reg:寄存器地址
* 返回参数：读到该寄存器序号里的值
* 说    明：       
****************************************************************************/	
uint16_t LCD_ReadReg(uint16_t LCD_Reg){
  LCD_CMD=LCD_Reg;
  DWT_Delay_us(4);
  return LCD_DAT;
}

/****************************************************************************
* 名    称: void LCD_Open_Window(u16 X0,u16 Y0,u16 width,u16 height)
* 功    能：开窗口,并设置画点坐标到窗口左上角(X0,Y0)
* 入口参数：X0,Y0:窗口起始坐标(左上角)
            width,height:窗口宽度和高度
* 返回参数：无
* 说    明：窗体大小:width*height.?
****************************************************************************/
void LCD_Open_Window(uint16_t X0,uint16_t Y0, uint16_t width, uint16_t height){
  width=X0+width-1;
  height=Y0+height-1;       // 算出右下角坐标
  LCD_CMD=set_xcmd;
  LCD_DAT=X0>>8; 
	LCD_DAT=X0&0XFF;	 
	LCD_DAT=width>>8; 
	LCD_DAT=width&0XFF;  
	LCD_CMD=set_ycmd; 
	LCD_DAT=Y0>>8; 
	LCD_DAT=Y0&0XFF; 
	LCD_DAT=height>>8; 
	LCD_DAT=height&0XFF;
}

/****************************************************************************
* 名    称: void Set_Scan_Direction(u8 direction)    启￥明#欣￥欣
* 功    能：设置LCD的扫描方向
* 入口参数：direction：扫描方向
* 返回参数：无
* 说    明：     
****************************************************************************/
void Set_Scan_Direction(uint8_t direction){
  uint16_t skhda=0;
  uint16_t diomf=0;
  // 9341横屏需要转换一下
  if(dir_flag==1&&lcd_id==0x9341){
    switch(direction){
      case 0:direction=6;break;
      case 1:direction=7;break;
      case 2:direction=4;break;
      case 3:direction=5;break;
      case 4:direction=1;break;
      case 5:direction=0;break;
      case 6:direction=3;break;
      case 7:direction=2;break;
    }
  }
  switch(direction){
    case L2R_U2D:
      skhda|=(0<<7)|(0<<6)|(0<<5);
      break;
    case L2R_D2U:
      skhda|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D:   //从右到左,从上到下
			skhda|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U:   //从右到左,从下到上
			skhda|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R:   //从上到下,从左到右
			skhda|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L:   //从上到下,从右到左
			skhda|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R:   //从下到上,从左到右
			skhda|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L:   //从下到上,从右到左
			skhda|=(1<<7)|(1<<6)|(1<<5); 
			break;
  }
  diomf=0x36;
  skhda|=0x08;
  LCD_WriteReg(diomf,skhda);
  LCD_Open_Window(0,0,lcd_width,lcd_height);
}

/****************************************************************************
* 名    称: void LCD_SetCursor(u16 Xaddr, u16 Yaddr)       
* 功    能：设置光标位置
* 入口参数：x：x坐标
            y：y坐标
* 返回参数：无
* 说    明：     
****************************************************************************/
void LCD_SetCursor(uint16_t Xaddr, uint16_t Yaddr)
{
  LCD_CMD = set_xcmd;
  LCD_DAT = (Xaddr >> 8);
  LCD_DAT = (Xaddr & 0XFF);
  LCD_CMD = set_ycmd;
  LCD_DAT = (Yaddr >> 8);
  LCD_DAT = (Yaddr & 0XFF);
}

/****************************************************************************
* 名    称: void Set_Display_Mode(u8 mode)
* 功    能：设置LCD显示方向
* 入口参数：mode: 0,竖屏
                  1,横屏
* 返回参数：无
* 说    明：?
****************************************************************************/
void Set_Display_Mode(uint8_t mode){
  if (mode == 0){
    dir_flag = 0;
    write_gramcmd = 0x2C;
    set_xcmd = 0x2A;
    set_ycmd = 0x2B;
    lcd_width = MY_DISP_VER_RES;
    lcd_height = MY_DISP_HOR_RES;
    LCD_CMD = 0x2B;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0x01;LCD_DAT = 0x3f;
    LCD_CMD = 0x2A;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0xef;
  }else{
    dir_flag = 1;
    write_gramcmd = 0x2C;
    set_xcmd = 0x2A;
    set_ycmd = 0x2B;
    lcd_width = MY_DISP_HOR_RES;
    lcd_height = MY_DISP_VER_RES;
    LCD_CMD = 0x2B;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0xef;
    LCD_CMD = 0x2A;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0x01;LCD_DAT = 0x3f;
  }
  Set_Scan_Direction(L2R_U2D);
}

/****************************************************************************
* 名    称: void LCD_Clear(u16 color)
* 功    能：清屏函数
* 入口参数：color: 要清屏的填充色
* 返回参数：无
* 说    明：?
****************************************************************************/
void LCD_Clear(uint16_t color)
{
	uint32_t i=0;      
	uint32_t pointnum=0;
	
	pointnum=lcd_width*lcd_height; 	 //得到LCD总点数
	LCD_SetCursor(0x00,0x00);	       //设置光标位置 
	LCD_WriteGRAM();     		 //开始写入GRAM	 	  
	for(i=0;i<pointnum;i++)
	{
		LCD_DAT=color;	   
	}
}

void LCD_Init(void){
  Backlight_Pin_Init();
  MX_FSMC_Init();
  DWT_Delay_ms(50);
  lcd_id=ILI9341_Read_id();
  FSMC_Bank1E->BWTR[3]&=~(0xF<<0);      // 地址建立时间清零
  FSMC_Bank1E->BWTR[3]&=~(0xF<<8);      // 数据保存时间清零
  FSMC_Bank1E->BWTR[3]|=3<<0;           // 地址建立时间为3个HCLK=18ns
  FSMC_Bank1E->BWTR[3]|=2<<8;           // 数据保存时间为6ns*3个HCLK=18ns

  LCD_CMD = 0xCF;LCD_DAT = 0x00;LCD_DAT = 0xC1;LCD_DAT = 0X30;
  LCD_CMD = 0xED;LCD_DAT = 0x64;LCD_DAT = 0x03;LCD_DAT = 0X12;LCD_DAT = 0X81;
  LCD_CMD = 0xE8;LCD_DAT = 0x85;LCD_DAT = 0x10;LCD_DAT = 0x7A;
  LCD_CMD = 0xCB;LCD_DAT = 0x39;LCD_DAT = 0x2C;LCD_DAT = 0x00;LCD_DAT = 0x34;LCD_DAT = 0x02;
  LCD_CMD = 0xF7;LCD_DAT = 0x20;
  LCD_CMD = 0xEA;LCD_DAT = 0x00;LCD_DAT = 0x00;
  LCD_CMD = 0xC0;LCD_DAT = 0x1B;
  LCD_CMD = 0xC1;LCD_DAT = 0x01;
  LCD_CMD = 0xC5;LCD_DAT = 0x30;LCD_DAT = 0x30;
  LCD_CMD = 0xC7;LCD_DAT = 0XB7;
  LCD_CMD = 0x36;LCD_DAT = 0x48;
  LCD_CMD = 0x3A;LCD_DAT = 0x55;
  LCD_CMD = 0xB1;LCD_DAT = 0x00;LCD_DAT = 0x1A;
  LCD_CMD = 0xB6;LCD_DAT = 0x0A;LCD_DAT = 0xA2;
  LCD_CMD = 0xF2;LCD_DAT = 0x00;
  LCD_CMD = 0x26;LCD_DAT = 0x01;
  LCD_CMD = 0xE0;LCD_DAT = 0x0F;LCD_DAT = 0x2A;LCD_DAT = 0x28;LCD_DAT = 0x08;LCD_DAT = 0x0E;LCD_DAT = 0x08;
    LCD_DAT = 0x54;LCD_DAT = 0XA9;LCD_DAT = 0x43;LCD_DAT = 0x0A;LCD_DAT = 0x0F;LCD_DAT = 0x00;LCD_DAT = 0x00;
    LCD_DAT = 0x00;LCD_DAT = 0x00;
  LCD_CMD = 0XE1;LCD_DAT = 0x00;LCD_DAT = 0x15;LCD_DAT = 0x17;LCD_DAT = 0x07;LCD_DAT = 0x11;LCD_DAT = 0x06;
    LCD_DAT = 0x2B;LCD_DAT = 0x56;LCD_DAT = 0x3C;LCD_DAT = 0x05;LCD_DAT = 0x10;LCD_DAT = 0x0F;LCD_DAT = 0x3F;
    LCD_DAT = 0x3F;LCD_DAT = 0x0F;
  // LCD_CMD = 0x2B;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0x01;LCD_DAT = 0x3f;
  // LCD_CMD = 0x2A;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0x00;LCD_DAT = 0xef;
  LCD_CMD = 0x11;
  DWT_Delay_ms(120);
  LCD_CMD = 0x29;

  BackLight = 1; // 点亮背光
  Set_Display_Mode(0);          // 初始化为横屏
  LCD_Clear(WHITE);             // 清屏白色
}

/****************************************************************************
* 名    称: void LCD_DrawPoint(u16 x,u16 y)
* 功    能：画点（在该点写入画笔的颜色）
* 入口参数：x：x坐标
          y：y坐标
* 返回参数：无
* 说    明?RUSH_COLOR:此点的颜色值
****************************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y){
  LCD_SetCursor(x,y);   // 设置光标位置
  LCD_WriteGRAM();        // 开始写入GRAM
  LCD_DAT=BRUSH_COLOR;
}

/****************************************************************************
* 名    称: LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
* 功    能：使用 Bresenham 算法画直线
****************************************************************************/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color){
  // 1. 【新增】边界检查：如果起点或终点超出屏幕，直接返回，防止花屏
  if (x1 >= lcd_width || y1 >= lcd_height || x2 >= lcd_width || y2 >= lcd_height) return;
  int16_t dx,dy;
  int16_t sx,sy;
  int16_t err,e2;
  // 2. 【修复】核心步骤：必须先计算差值！
  dx=x2-x1;                         // X轴总共要走多远（可能是负数，表示向左）
  dy=y2-y1;                         // Y轴总共要走多远（可能是负数，表示向上）
  // 3. 计算方向和绝对值
  sx=(dx>0)?1:-1;                   // 步长方向：如果向右走，sx=1；如果向左走，sx=-1
  sy=(dy>0)?1:-1;                   // 步长方向：如果向下走，sy=1；如果向上走，sy=-1
  // 取绝对值
  dx=(dx>0)?dx:-dx;                 // 取绝对值，只关心距离，不关心方向了
  dy=(dy>0)?dy:-dy;
  // 初始化误差值
  // 意思：err (Error) 是一个“累积误差”
  // 理想直线通常不会正好穿过像素中心。这个变量用来记录：“我现在偏离理想直线有多远了？”
  err=dx-dy;
  while(1){
    // 1. 先在当前位置画个点
    LCD_Color_DrawPoint(x1,y1,color);     
    // 2. 检查是否到达终点
    if((x1==x2)&&(y1==y2))  break;          // 到了就退出循环！
    e2=2*err;                               // 误差翻倍（为了避开浮点数除法，这是算法技巧）
    // 3. 决定下一步怎么走
    if(e2>-dy){
      // 如果误差偏向X轴太多 -> X轴走一步，修正误差
      err-=dy;
      x1+=sx;
    }
    if(e2<dx){
      // 如果误差偏向Y轴太多 -> Y轴走一步，修正误差
      err+=dx;
      y1+=sy;
    }
  }
}

/****************************************************************************
* 名    称: void LCD_Color_DrawPoint(u16 x,u16 y,u16 color)
* 功    能：在设置的坐标处画相应颜色（在该点写入自定义颜色）
* 入口参数：x：x坐标
            y：y坐标
            color 此点的颜色值
* 返回参数：无
* 说    明：color:写入此点的颜色值   GUI调用该函数
****************************************************************************/
void LCD_Color_DrawPoint(uint16_t x, uint16_t y, uint16_t color){
  LCD_DrawPoint(x,y);
  LCD_CMD=write_gramcmd;
  LCD_DAT=color;
}

/****************************************************************************
* 名    称: void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)  启*明@欣#欣
* 功    能：在指定区域内填充单个颜色
* 入口参数：(sx,sy),(ex,ey):填充矩形对角坐标
            color:要填充的颜色
* 返回参数：无
* 说    明：区域大小为:(ex-sx+1)*(ey-sy+1)  ?
****************************************************************************/
void LCD_Fill_onecolor(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color){
  uint16_t i,j;
  uint16_t nlen=0;
  nlen=ex-sx+1;
  for(i=sy;i<=ey;i++){
    LCD_SetCursor(sx,i);
    LCD_WriteGRAM();
    for(j=0;j<nlen;j++){
      LCD_DAT=color;
    }
  }
}

/****************************************************************************
* 名    称: void LCD_Draw_Picture(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
* 功    能：在指定区域内画入图片
* 入口参数：(sx,sy),(ex,ey):填充矩形对角坐标
            color:要填充的图片像素颜色数组
* 返回参数：无
* 说    明：区域大小为:(ex-sx+1)*(ey-sy+1)  ?
****************************************************************************/
void LCD_Draw_Picture(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t* color){
  uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			    //得到图片的宽度
	height=ey-sy+1;			    //得到图片的高度
  for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i); //设置光标位置 
		LCD_WriteGRAM();        //开始写入GRAM
		for(j=0;j<width;j++)
      LCD_DAT=color[i*height+j];//写入颜色值
	}	
}

/****************************************************************************
* 名    称: void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size)
* 功    能：在指定位置显示一个字符
* 入口参数：x,y:起始坐标
            word:要显示的字符:abcdefg1234567890...
            size:字体大小 12/16/24
* 返回参数：无
* 说    明：该字模取模方向为先从左到右，再从上到下  低位在前  ?
****************************************************************************/
void LCD_DisplayChar(uint16_t x,uint16_t y,uint8_t word,uint8_t size){
  uint8_t bytenum,bytedata,a,b;
  uint16_t xmid=x;  // x初始位置
  if(size==12) bytenum=12;        // 从字库数组中可知道每种字体单个字符所占的字节数
  else if(size==16) bytenum=16;
  else if(size==24) bytenum=48;
  else return;

  word=word-' ';                  // 字库数组是按ASCII表排列
  // cfont.h中字库是从空格开始的 空格就是第一个元素 其他字符的ASCII码减去空格后就得到在数组中的偏移值(位置) 
  for(b=0;b<bytenum;b++){
    if(size==12) bytedata=char_1206[word][b];
    else if(size==16) bytedata=char_1608[word][b];
    else if(size==24) bytedata=char_2412[word][b];
    
    for(a=0;a<8;a++){
      if(bytedata&0x01) LCD_Color_DrawPoint(x,y,BRUSH_COLOR);     // 由于子模是低位在前 所以先从低位判断  为1时显示画笔颜色
      else LCD_Color_DrawPoint(x,y,BACK_COLOR);                   // 0时显示背景颜色
      bytedata>>=1;                                               // 低位判断完 依次往高位判断
      x++;                                                        // 显示完一位 往下一位显示
      if((x-xmid)==size/2){                                       // x方向超出字体大小 如：16字体 实际是 08*16的点阵  故这边 size/2
        x=xmid;                                                   // 从初始X位置在写下一行
        y++;                                                      // 上一行写完 从下一行再写
        break;                                                    // 跳出for(a=0;a<8;a++)循环
      }
    }
  }
}

/****************************************************************************
* 名    称: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* 功    能：显示字符串
* 入口参数：x,y:起点坐标
*           size:字体大小
*           *p:字符串起始地址	
* 返回参数：无
* 说    明：  ?
****************************************************************************/	
void LCD_DisplayString(uint16_t x,uint16_t y,uint8_t size,uint8_t* p){
  while((*p>=' ')&&(*p<='~')) //只显示“ ”到“~”之间的字符
  {       
    LCD_DisplayChar(x,y,*p,size);
    x+=size/2;
    if(x>=lcd_width) break;
    p++;
  }
}

/****************************************************************************
* 名    称: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* 功    能：显示自定义字符串
* 入口参数：x,y:起点坐标
*           width,height:区域大小  
*           size:字体大小
*           *p:字符串起始地址	
*           brushcolor：自定义画笔颜色
*           backcolor： 自定义背景颜色
* 返回参数：无
* 说    明：  ?
****************************************************************************/	
void LCD_DisplayString_color(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint16_t brushcolor,uint16_t backcolor)
{
  uint16_t bh_color,bk_color;
	
 	bh_color=BRUSH_COLOR;  //暂存画笔颜色
	bk_color=BACK_COLOR;   //暂存背景颜色
	
	BRUSH_COLOR=brushcolor;
	BACK_COLOR=backcolor;
	
	LCD_DisplayString(x,y,size,p);
	
	BRUSH_COLOR=bh_color;   //不改变系统颜色
	BACK_COLOR=bk_color;
}

//a^n函数，返回值:a^n次方
uint32_t Counter_Power(uint8_t a,uint8_t n)
{
	uint32_t mid=1;	 
	while(n--)mid*=a;    
	return mid;
}

/****************************************************************************
* 名    称: void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* 功    能：在指定位置显示一串数字
* 入口参数：x,y:起点坐标
            value:数值;	 
            len:长度(设置显示的位数)
            size:字体大小
            mode: 0：高位为0不显示
                  1：高位为0根据len长度补显示几个0
* 返回参数：无
* 说    明：  ?
****************************************************************************/
void LCD_DisplayNum(uint16_t x, uint16_t y, uint32_t value, uint8_t len, uint8_t size, uint8_t mode)
{
  uint8_t t, numtemp;
  uint8_t value_num; // 数值总共几位数
  uint32_t value_mid;

  value_mid = value; // 计算位数时不影响要显示的数值大小
  for (value_num = 0; value_mid > 0; value_num++)
  {
    value_mid /= 10;
  } // 执行完for，就知道要显示的数值为几位数

  if (value_num > len) // 数值位数大于设置位数，即显示区域不够 显示错误
  {
    LCD_DisplayString(x, y, size, "ERROR");
    return; // 退出函数
  }
  else
  {
    for (t = 0; t < len; t++)
    {
      if (t < (len - value_num))
      {
        if (mode)
          LCD_DisplayChar(x + (size / 2) * t, y, '0', size);
        else
          LCD_DisplayChar(x + (size / 2) * t, y, ' ', size);
      }
      else
      {
        numtemp = (value / Counter_Power(10, len - t - 1)) % 10; // 取出各位数值
        LCD_DisplayChar(x + (size / 2) * t, y, numtemp + '0', size);
      }
    }
  }
}

/****************************************************************************
* 名    称: void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* 功    能：在指定位置显示一串自定义颜色的数字  启#明*欣&欣
* 入口参数：x,y:起点坐标
            num:数值;	 
            len:长度(即要显示的位数)
            size:字体大小
            mode: 0：高位为0不显示
                  1：高位为0显示0
            brushcolor：自定义画笔颜色
            backcolor： 自定义背景颜色
* 返回参数：无
* 说    明：  ?
****************************************************************************/
void LCD_DisplayNum_color(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode,uint16_t brushcolor,uint16_t backcolor)
{
  uint16_t bh_color,bk_color;
	
 	bh_color=BRUSH_COLOR;  //暂存画笔颜色
	bk_color=BACK_COLOR;   //暂存背景颜色
	
	BRUSH_COLOR=brushcolor;
	BACK_COLOR=backcolor;
	
	LCD_DisplayNum(x,y,num,len,size,mode);
	
	BRUSH_COLOR=bh_color;   //不改变系统颜色
	BACK_COLOR=bk_color;
}

//lcd延时函数
void lcdm_delay(uint8_t i)
{
	while(i--);
}

/****************************************************************************
* 名    称: u16 LCD_GetPoint(u16 x,u16 y)
* 功    能：读取某点的颜色值	
* 入口参数：x：x坐标
            y：y坐标
* 返回参数：此点的颜色
* 说    明：     
****************************************************************************/
uint16_t LCD_GetPoint(uint16_t x,uint16_t y){
  volatile uint16_t r = 0, g = 0, b = 0;

  LCD_SetCursor(x, y);

  LCD_CMD = 0X2E; // 9341与1963读GRAM指令一样
  r = LCD_DAT;

  if (lcd_id == 0X1963) return r; // 1963直接读出来就是16位颜色值

  else // 其他驱动就是9341
  {
    lcdm_delay(2);
    b = LCD_DAT;  // 9341要读2次
    g = r & 0XFF; // 9341第一次读取的是RG的值,R在前,G在后,各占8位
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); // 9341需公式转换
  }
}

void LCD_task(void* pvParameters){
  // LCD_Init();
  LCD_DisplayString(10,10,24,"Illuminati STM32");	
	LCD_DisplayString(20,40,16,"Author:Clever");
	LCD_DisplayString(30,80,24,"4.TFTLCD TEST");
	LCD_DisplayString(50,110,16,"KEY0:Screen Test");	
	LCD_DisplayString(50,130,16,"KEY1:Number Test");

  uint8_t count=0;
  while(1){
    LCD_DisplayString(50,150,16,"Count number: ");
    LCD_DisplayNum(155,150,count,3,16,1);
    count++;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
