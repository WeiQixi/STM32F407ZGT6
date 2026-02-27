#include "led.h"
#include "key.h"

void Led_task(void *pvParameters)
{
	if (key_down == KEY0_DATA)
	{
		LED0 = 0;
		LED1 = 0;
		LED2 = 0;
	}
	if (key_up == KEY1_DATA)
	{
		LED0 = 1;
	}
	if (key_tem == KEY2_DATA && key_time > 200)
	{
		LED1 = 1;
	}
	if (key_tem == KEY3_DATA && key_time > 500)
	{
		LED2 = 1;
	}
}
