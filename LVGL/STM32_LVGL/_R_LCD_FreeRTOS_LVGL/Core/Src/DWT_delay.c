#include "DWT_delay.h"

void DWT_Delay_us(uint32_t us){
  uint32_t start = DWT->CYCCNT;
  uint32_t freq = SystemCoreClock/1000000;
  if(freq==0) freq=168;
  while((DWT->CYCCNT-start)<us*freq){
    __NOP();
  }
  __DSB();
  __ISB();
}
void DWT_Delay_ms(uint32_t ms){
  DWT_Delay_us(ms*1000);
}
