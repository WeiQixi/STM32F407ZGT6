#ifndef __LED_H
#define __LED_H

#include "common.h"

#define LED0 PEout(3)
#define LED1 PEout(4)
#define LED2 PGout(9)

void Led_task(void *pvParameters);

#endif
