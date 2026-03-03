#include "led.h"
#include "key.h"
#include "main.h"
#include "DWT_delay.h"

void Led_task(void *pvParameters)
{
	while(1){
		if (key_down == KEY0_DATA)
		{
			LED0 = 0;
			LED1 = 0;
			LED2 = 0;
			BackLight=0;
		}
		if (key_up == KEY1_DATA)
		{
			LED0 = 1;
			BackLight=1;
		}
		if (key_tem == KEY2_DATA && key_time > 200)
		{
			LED1 = 1;
		}
		if (key_tem == KEY3_DATA && key_time > 400)
		{
			LED2 = 1;
		}
		vTaskDelay(pdMS_TO_TICKS(1));
		// DWT_Delay_ms(2);
	}
}
