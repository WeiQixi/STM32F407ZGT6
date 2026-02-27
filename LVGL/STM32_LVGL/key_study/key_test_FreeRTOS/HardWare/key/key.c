#include "key.h"
#include "main.h"

uint8_t key_down=0x00;
uint8_t key_up=0x00;
uint16_t key_time=0x00;

uint8_t key_tem=0x00;
uint8_t key_bak=0x00;

void key_scan(uint8_t mode){
	key_up=0;
	if(KEY0==0||KEY1==0||KEY2==0||KEY3==0){
		if(KEY0==0)	key_tem=1;
		else if (KEY1==0)	key_tem=2;
		else if (KEY2==0)	key_tem=3;
		else if (KEY3==0)	key_tem=4;
		if(key_tem==key_bak){
			key_time++;
			key_down=key_tem;
			if((mode==0)&&(key_time>1))	key_down=0;
		}else{
			key_time=0;
			key_bak=key_tem;
		}
	}else{
		if(key_time>2) key_up=key_tem;
		key_bak=0;
		key_time=0;
		key_down=0;
	}
}

void Scan_Task(void* pvParameters){
	uint8_t mode=*((uint8_t*)pvParameters);
	while(1){
		//
		key_up=0;
		if(KEY0==0||KEY1==0||KEY2==0||KEY3==0){
			if(KEY0==0)	key_tem=1;
			else if (KEY1==0)	key_tem=2;
			else if (KEY2==0)	key_tem=3;
			else if (KEY3==0)	key_tem=4;
			if(key_tem==key_bak){
				key_time++;
				key_down=key_tem;
				if((mode==0)&&(key_time>20))	key_down=0;
			}else{
				key_time=0;
				key_bak=key_tem;
			}
		}else{
			if(key_time>2) key_up=key_tem;
			key_bak=0;
			key_time=0;
			key_down=0;
		}
		vTaskDelay(pdMS_TO_TICKS(5));
	}
}
