#include "touch.h"
#include "lcd.h"

void Touch_Init(void){
  XPT2046_Init();
}

void Clear_Screen(void){
  LCD_Clear(WHITE);
  BRUSH_COLOR=BLUE;
  LCD_DisplayString(lcd_width-40,lcd_height-18,16,"Clear");
  BRUSH_COLOR=RED;
}

/****************************************************************************
* 名    称: u8 Draw_Point(u16 x,u16 y,u16 color)
* 功    能：画一个点(4*4的点)	
* 入口参数：x,y:坐标
            color:颜色
* 返回参数: 无  
* 说    明：        
****************************************************************************/
void Draw_Point(uint16_t x,uint16_t y,uint16_t color)
{	    	
	uint8_t i=0;

	BRUSH_COLOR=color;
	for(i=0;i<4;i++)
	{
    // LCD_DrawPoint(x, y + i);
    LCD_Color_DrawPoint(x,y+i,BLUE);
    LCD_DrawPoint(x + 1, y + i);
    // LCD_DrawPoint(x + 2, y + i);
    LCD_Color_DrawPoint(x+2,y+i,BLUE);
    LCD_DrawPoint(x + 3, y + i);
  } 	  	
}

//电阻触摸屏测试函数
void R_Touch_test(void* pvParameters)
{
  LCD_Init();
  // Set_Display_Mode(0);
  Touch_Init();
  Clear_Screen();
  LCD_DisplayString(10,10,16,"Touch State:");	
	LCD_DisplayString(10,40,16,"x:");
	LCD_DisplayString(10,70,16,"y:");
  // 2. 【新增变量】用于记录“上一个点”的坐标，实现连线
  uint16_t last_x=0,last_y=0;
  uint8_t is_first_point=1;   // 标记：是否是这一笔的第一个点

  // 3. 【新增变量】用于记录“上一次显示的数值”，实现变化才刷新（防闪、提速）
  uint16_t show_x=0,show_y=0;
  uint16_t last_show_x=0xFFFF,last_show_y=0xFFFF;
  uint8_t last_state=2;       // 0:Release, 1:Pressed, 2:Unknown

  while(1){
    XPT2046_Scan(0);
    // --- A. 获取当前有效坐标 ---
    uint8_t is_pressed = (XDown != 0xFFFF) && (YDown != 0xFFFF) && 
                          (XDown < lcd_width) && (YDown < lcd_height);
    if(is_pressed){
      show_x=XDown;
      show_y=YDown;
      // 限幅处理
      if(dir_flag){
        if(show_x>319) show_x=319;
        if(show_y>239) show_y=239;
      }else{
        if(show_x>239) show_x=239;
        if(show_y>319) show_y=319;
      }
      
    }else{
      show_x=0;
      show_y=0;
    }
    // --- B. 刷新状态文字 (仅当状态改变时) ---
    if(is_pressed&&last_state!=1){
      LCD_DisplayString_color(110, 10, 16, "Pressed", RED, WHITE);
      last_state = 1;
    }else if(!is_pressed && last_state != 0){
      LCD_DisplayString_color(110, 10, 16, "Release", BLUE, WHITE);
      last_state = 0;
    }
    // --- C. 刷新数字显示 (仅当数值改变时，解决闪烁和卡顿的核心) ---
    if (show_x != last_show_x) {
        LCD_Fill_onecolor(30, 40, 80, 55, WHITE); // 只清空数字区域
        LCD_DisplayNum(30, 40, show_x, 3, 16, 1);
        last_show_x = show_x;
    }
    if (show_y != last_show_y) {
        LCD_Fill_onecolor(30, 70, 80, 85, WHITE); // 只清空数字区域
        LCD_DisplayNum(30, 70, show_y, 3, 16, 1);
        last_show_y = show_y;
    }
    // --- D. 核心画线逻辑 (解决虚线问题) ---
    if(is_pressed){
      // 检查是否点击了右下角清屏区
      if (show_x > (lcd_width - 40) && show_y > (lcd_height - 18)) {
        Clear_Screen(); // 清屏
        // ?【关键修复】清屏后必须重新画静态文字！否则字就没了！
        LCD_DisplayString(10,10,16,"Touch State:");	
        LCD_DisplayString(10,40,16,"x:");
        LCD_DisplayString(10,70,16,"y:");
        // 重置所有状态，准备重新开始
        last_state = 2;
        last_show_x = 0xFFFF; 
        last_show_y = 0xFFFF;
        is_first_point = 1; 
      }else{
        // 正常画点/画线
        if(is_first_point){
          // 第一个点：只能画点，因为没有“上一个点”可以连
          Draw_Point(show_x, show_y, RED);
          is_first_point = 0; // 标记后续点不是第一个
        }else{
          // 后续点：调用画线函数，连接“上一个点”和“当前点” -> 形成实线
          LCD_DrawLine(last_x, last_y, show_x, show_y, RED);
        }
        // 更新“上一个点”为当前点，供下一次循环使用
        last_x=show_x;
        last_y=show_y;
      }
    }else{
      // 手指抬起：重置“第一个点”标记，下一笔开始时重新画单点
      is_first_point=1;
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
