#include <reg52.h>
#include "main.h"
#include "bsp_delay.h"
#include "bsp_key.h"
#include "bsp_ds1302.h"

/* 定义控制数码管的引脚，这里使用宏定义形式是便于以后程序的移植 */
#define Segment P0
#define Digital P2

/* 定义模型变量 */
u8 location = 0;								// 记录当期是哪一位闪烁
u8 value[4] = {1, 2, 3, 4};							// 记录当前数码管显示的值

/* 共阴极数码管段选码、位选码 */
u8 SegCode[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
u8 DigCode[4] = {0xfe, 0xfd, 0xfb, 0xf7};

int main()
{
        u8 i, j;
        u8 tmp1, tmp2;
        
        /* 这里设置的值是表示，10次扫描全部显示，10次扫描location位不显示 */
        u8 flickerCount = 20;
        
        IT0 = 1;
        IE = 0x81;
        
        DS1302_WriteRegister(0x8E,0X00);
        
        while(1)
        {
                tmp1 = DS1302_ReadRegister(DS1302_MIN | DS1302_READ);
                tmp2 = DS1302_ReadRegister(DS1302_SEC | DS1302_READ);

                value[0] = (tmp1 & 0xf0) >> 4;
                value[1] = tmp1 & 0x0f;
                
                value[2] = (tmp2 & 0xf0) >> 4;
                value[3] = tmp2 & 0x0f;
                
                /* 这里扫描的时候是正常显示4位 */
                for(j = flickerCount; j > 0; j--)
                {
                        for(i = 0; i < 4; i++)
                        {
                                Digital = DigCode[i];
                                
                                /* 这是是在第一位位置上添加小数点 */
                                if(i == 1)
                                {
                                        /* 通过按位与操作，在不改变低7位的前提下，将位7置为高电平 */
                                        Segment = SegCode[value[i]] | 0x80;
                                }
                                else
                                {
                                        Segment = SegCode[value[i]];
                                }
                                delay_us(50);
                        }
                }
                
                /* 这里扫描的时候location对应位不显示，只显示其他位 */
                for(j = flickerCount; j > 0; j--)
                {
                        for(i = 0; i < 4; i++)
                        {
                                /* 判断当期扫描的位是不是location对应位 */
                                if(location == i)
                                {
                                        /* 如果是的话，不选中任何位选，也就是全灭 */
                                        Digital = 0xff;
                                }
                                else
                                {
                                        /* 对于其他为就正常显示就好 */
                                        Digital = DigCode[i];
                                }
                                
                                /* 这里和上面是一样的，加小数点的操作 */
                                if(i == 1)
                                {
                                        Segment = SegCode[value[i]] | 0x80;
                                }
                                else
                                {
                                        Segment = SegCode[value[i]];
                                }
                                delay_us(50);
                        }
                }
                
        }
}

/**
 * @Descroption 外部中断0服务函数
 */ 
void INT0_Handler() interrupt 0
{
        Key_Control(Key_Scan());
}
