#ifndef __CFONT_H
#define __CFONT_H 

//常用ASCII表，数字与字母
//偏移量32，空格之后字符
//从字库数组中可知道每种字体单个字符所占的字节数

//大小:12*6
extern const unsigned char char_1206[95][12];

//大小:16*8
extern const unsigned char char_1608[95][16];

//大小:24*8
extern const unsigned char char_2412[95][48];

#endif
