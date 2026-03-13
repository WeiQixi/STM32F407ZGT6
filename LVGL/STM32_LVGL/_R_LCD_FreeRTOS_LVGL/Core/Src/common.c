#include "common.h"

/****************************************************************************
* 名    称: void GPIO_group_OUT(_gpio_group *group,u16 outdata)
* 功    能：使用随意16个IO口组成一个16位并行输出口
* 入口参数：*group： 任意16个IO口为元素的结构体指针
            outdata: 16位输出数值
* 返回参数：无
* 说    明：outdata从高位开始赋值
****************************************************************************/
void GPIO_Group_OUT(_gpio_group* group, uint16_t outdata){
	// uint8_t count;
	for(uint8_t count=0;count<16;count++){
		if((outdata&0x8000)>>15){
			switch(count){
				case 0:   group->data15=1; break;
				case 1:   group->data14=1; break;
				case 2:   group->data13=1; break;
				case 3:   group->data12=1; break;
				case 4:   group->data11=1; break;
				case 5:   group->data10=1; break;
				case 6:   group->data9=1;  break;
				case 7:   group->data8=1;  break;
				case 8:   group->data7=1;  break;
				case 9:   group->data6=1;  break;
				case 10:  group->data5=1;  break;
				case 11:  group->data4=1;  break;
				case 12:  group->data3=1;  break;
				case 13:  group->data2=1;  break;
				case 14:  group->data1=1;  break;
				case 15:  group->data0=1;  break;
			}
		}else{
			switch(count){
				case 0:   group->data15=0; break;
				case 1:   group->data14=0; break;
				case 2:   group->data13=0; break;
				case 3:   group->data12=0; break;
				case 4:   group->data11=0; break;
				case 5:   group->data10=0; break;
				case 6:   group->data9=0;  break;
				case 7:   group->data8=0;  break;
				case 8:   group->data7=0;  break;
				case 9:   group->data6=0;  break;
				case 10:  group->data5=0;  break;
				case 11:  group->data4=0;  break;
				case 12:  group->data3=0;  break;
				case 13:  group->data2=0;  break;
				case 14:  group->data1=0;  break;
				case 15:  group->data0=0;  break;
			}
		}
		outdata<<=1;
	}
}

/****************************************************************************
* 名    称: void GPIO_bits_OUT(GPIO_TypeDef* GPIOx, u8 start_bit, u8 bit_size,u16 outdata)
* 功    能：位段操作实现，同一IO口的几位并行输出操作
* 入口参数：* GPIOx：  对应的IO口
*           start_bit: 并行输出的起始位
*           bit_size:  要并行输出的位数
* 返回参数：无
* 说    明：start_bit: 0~14
            bit_size:  1~16 
            bit_size<=16-start_bit
****************************************************************************/
void GPIO_Bits_OUT(GPIO_TypeDef* GPIOx, uint8_t start_bit, uint8_t bit_size, uint16_t outdata){
	uint32_t mask;
	uint32_t temp;

	// 1. 参数边界保护（防止越界）
	if(bit_size>16-start_bit) bit_size=16-start_bit;
	// 2. 构造掩码
	uint32_t low_mask=(start_bit==0)?0:((1UL<<start_bit)-1);
	uint32_t high_mask=(bit_size>=16)?0:((~0UL<<bit_size+start_bit));
	// 合并掩码：高位保留+低位保留=中间清零
	mask=high_mask|low_mask;
	// 3. 执行原子操作：先清零，在置位
	// 关闭中断可确保绝对原子性（如果在多任务/中断环境中）
  	// 普通应用直接写即可，ODR 操作本身很快
	temp=GPIOx->ODR;	// 读取当前值
	temp&=mask;				// 清零目标位
	temp|=((uint32_t)outdata<<start_bit);	// 填入新数据
	GPIOx->ODR=temp;
}
