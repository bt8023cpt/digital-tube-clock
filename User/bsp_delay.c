#include "bsp_delay.h"

/** 
 * @Description 通过软件延时实现的简单延时函数
 * @param t 参数并不代表具体的时间
 */
void delay_ms(u32 t)
{
        u32 i, j;
        for(i = t; i > 0; i--)
                for(j = 100; j > 0; j--);
}
