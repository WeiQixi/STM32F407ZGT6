#ifndef __KEY_H
#define __KEY_H

#include "common.h"

#define KEY0	PFin(9)
#define KEY1	PFin(8)
#define KEY2	PFin(7)
#define KEY3	PFin(6)

#define KEY0_DATA	1
#define KEY1_DATA	2
#define KEY2_DATA	3
#define KEY3_DATA	4

extern uint8_t key_down;
extern uint8_t key_up;
extern uint16_t key_time;
extern uint8_t key_tem;

void key_scan(uint8_t mode);
void Scan_Task(void* pvParameters);

#endif
